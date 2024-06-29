/**
 * \file      CVTrackedMediaPipeHands.cpp
 * \date      December 2022
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifdef SL_BUILD_WITH_MEDIAPIPE
#    include <CVTrackedMediaPipeHands.h>

//-----------------------------------------------------------------------------
#    define CHECK_MP_RESULT(result) \
        if (!result) \
        { \
            const char* error = mp_get_last_error(); \
            std::cerr << error << std::endl; \
            mp_free_error(error); \
            SL_EXIT_MSG("Exiting due to MediaPipe error"); \
        }
//-----------------------------------------------------------------------------
typedef std::vector<std::pair<mp_hand_landmark, mp_hand_landmark>> ConnectionList;
//-----------------------------------------------------------------------------
// Defines the connection list used for drawing the hand skeleton
static const ConnectionList CONNECTIONS = {
  {mp_hand_landmark_wrist, mp_hand_landmark_thumb_cmc},
  {mp_hand_landmark_thumb_cmc, mp_hand_landmark_thumb_mcp},
  {mp_hand_landmark_thumb_mcp, mp_hand_landmark_thumb_ip},
  {mp_hand_landmark_thumb_ip, mp_hand_landmark_thumb_tip},
  {mp_hand_landmark_wrist, mp_hand_landmark_index_finger_mcp},
  {mp_hand_landmark_index_finger_mcp, mp_hand_landmark_index_finger_pip},
  {mp_hand_landmark_index_finger_pip, mp_hand_landmark_index_finger_dip},
  {mp_hand_landmark_index_finger_dip, mp_hand_landmark_index_finger_tip},
  {mp_hand_landmark_index_finger_mcp, mp_hand_landmark_middle_finger_mcp},
  {mp_hand_landmark_middle_finger_mcp, mp_hand_landmark_middle_finger_pip},
  {mp_hand_landmark_middle_finger_pip, mp_hand_landmark_middle_finger_dip},
  {mp_hand_landmark_middle_finger_dip, mp_hand_landmark_middle_finger_tip},
  {mp_hand_landmark_middle_finger_mcp, mp_hand_landmark_ring_finger_mcp},
  {mp_hand_landmark_ring_finger_mcp, mp_hand_landmark_ring_finger_pip},
  {mp_hand_landmark_ring_finger_pip, mp_hand_landmark_ring_finger_dip},
  {mp_hand_landmark_ring_finger_dip, mp_hand_landmark_ring_finger_tip},
  {mp_hand_landmark_ring_finger_mcp, mp_hand_landmark_pinky_mcp},
  {mp_hand_landmark_wrist, mp_hand_landmark_pinky_mcp},
  {mp_hand_landmark_pinky_mcp, mp_hand_landmark_pinky_pip},
  {mp_hand_landmark_pinky_pip, mp_hand_landmark_pinky_dip},
  {mp_hand_landmark_pinky_dip, mp_hand_landmark_pinky_tip}};
//-----------------------------------------------------------------------------
CVTrackedMediaPipeHands::CVTrackedMediaPipeHands(SLstring dataPath)
{
    mp_set_resource_dir(dataPath.c_str());

    SLstring graphPath = dataPath + "mediapipe/modules/hand_landmark/hand_landmark_tracking_cpu.binarypb";
    auto*    builder   = mp_create_instance_builder(graphPath.c_str(), "image");

    // The following lines set some parameters for the tracking.
    // These are taken from MediaPipe's Python API:
    // https://github.com/google/mediapipe/blob/master/mediapipe/python/solutions/hands.py

    // Enable using the tracking results from the last frame to guess where the hand is now.
    // This speeds up tracking considerably.
    // Corresponds to "not static_image_mode" in the Python API.
    mp_add_side_packet(builder,
                       "use_prev_landmarks",
                       mp_create_packet_bool(true));

    // Maximum number of hands that can be detected.
    // Corresponds to "max_num_hands" in the Python API.
    mp_add_side_packet(builder,
                       "num_hands",
                       mp_create_packet_int(2));

    // Complexity of the hand landmark model, can be 0 or 1.
    // 0: Low landmark accuracy, fast
    // 1: High landmark accuracy, slow
    // Corresponds to "model_complexity" in the Python API.
    mp_add_side_packet(builder,
                       "model_complexity",
                       mp_create_packet_int(1));

    // Minimum confidence value for hand detection to be considered sucessful.
    // Corresponds to "min_detection_confidence" in the Python API.
    mp_add_option_float(builder,
                        "palmdetectioncpu__TensorsToDetectionsCalculator",
                        "min_score_thresh",
                        0.5);

    // Minimum confidence value for the hand landmarks to be considered tracked successfully.
    // Corresponds to "min_tracking_confidence" in the Python API.
    mp_add_option_double(builder,
                         "handlandmarkcpu__ThresholdingCalculator",
                         "threshold",
                         0.5);

    // Creates a MediaPipe instance with the graph and some extra info.
    _instance = mp_create_instance(builder);
    CHECK_MP_RESULT(_instance)

    // Creates a poller to read packets from an output stream.
    _landmarksPoller = mp_create_poller(_instance,
                                        "multi_hand_landmarks");
    CHECK_MP_RESULT(_landmarksPoller)

    // Starts the MediaPipe graph.
    CHECK_MP_RESULT(mp_start(_instance))

    // clang-format off
    // We define a identity matrix for the object view matrix because we do
    // not transform any object in the scenegraph so far.
    _objectViewMat = CVMatx44f(1,0,0,0,
                               0,1,0,0,
                               0,0,1,0,
                               0,0,0,1);
    // clang-format on
}
//-----------------------------------------------------------------------------
CVTrackedMediaPipeHands::~CVTrackedMediaPipeHands()
{
    mp_destroy_poller(_landmarksPoller);
    CHECK_MP_RESULT(mp_destroy_instance(_instance))
}
//-----------------------------------------------------------------------------
bool CVTrackedMediaPipeHands::track(CVMat          imageGray,
                                    CVMat          imageBgr,
                                    CVCalibration* calib)
{
    processImageInMediaPipe(imageBgr);

    if (mp_get_queue_size(_landmarksPoller) > 0)
    {
        auto* landmarksPacket = mp_poll_packet(_landmarksPoller);
        auto* landmarks       = mp_get_norm_multi_face_landmarks(landmarksPacket);

        drawResults(landmarks, imageBgr);

        mp_destroy_multi_face_landmarks(landmarks);
        mp_destroy_packet(landmarksPacket);
    }

    return true;
}
//-----------------------------------------------------------------------------
void CVTrackedMediaPipeHands::processImageInMediaPipe(CVMat imageBgr)
{
    cv::cvtColor(imageBgr, _imageRgb, cv::COLOR_BGR2RGB);

    mp_image inImage;
    inImage.data      = _imageRgb.data;
    inImage.width     = _imageRgb.cols;
    inImage.height    = _imageRgb.rows;
    inImage.format    = mp_image_format_srgb;
    mp_packet* packet = mp_create_packet_image(inImage);

    // Insert the image into the MediaPipe pipeline to be processed.
    CHECK_MP_RESULT(mp_process(_instance, packet))

    // Block until the results from MediaPipe are available.
    CHECK_MP_RESULT(mp_wait_until_idle(_instance))
}
//-----------------------------------------------------------------------------
//! Draws the hand skeleton with connections and joints into the RGB image
void CVTrackedMediaPipeHands::drawResults(mp_multi_face_landmark_list* landmarks,
                                          CVMat                        imageBgr)
{
    for (int i = 0; i < landmarks->length; i++)
    {
        auto& hand = landmarks->elements[i];

        for (auto& connection : CONNECTIONS)
        {
            auto  p1 = hand.elements[connection.first];
            auto  p2 = hand.elements[connection.second];
            float x1 = (float)imageBgr.cols * p1.x;
            float y1 = (float)imageBgr.rows * p1.y;
            float x2 = (float)imageBgr.cols * p2.x;
            float y2 = (float)imageBgr.rows * p2.y;

            cv::line(imageBgr,
                     {(int)x1, (int)y1},
                     {(int)x2, (int)y2},
                     CV_RGB(0, 255, 0),
                     2);
        }

        for (int j = 0; j < hand.length; j++)
        {
            auto  p      = hand.elements[j];
            float x      = (float)imageBgr.cols * p.x;
            float y      = (float)imageBgr.rows * p.y;
            float radius = std::max(3.0f + 25.0f * -p.z, 1.0f);

            cv::circle(imageBgr,
                       CVPoint((int)x, (int)y),
                       (int)radius,
                       CV_RGB(255, 0, 0),
                       -1);
        }
    }
}
//-----------------------------------------------------------------------------
#endif
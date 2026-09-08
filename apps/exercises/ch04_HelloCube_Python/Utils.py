###############################################################################
# File:       Utils.py
# Purpose:    Some shortcut helper functions
# Author:     Marcus Hudritsch
# Date:       10-AUG-25
# Copyright:  Marcus Hudritsch, Kirchrain 18, 2572 Sutz
#             THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL PURPOSE ONLY AND
#             WITHOUT ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED.
###############################################################################

import cv2
import numpy as np
import random
import sys
import matplotlib.pyplot as plt
import os
import zipfile
from pathlib import Path
from urllib.parse import urlparse
from urllib.request import urlretrieve
from typing import Union

# No torch import here on purpose: with the neural network helpers moved to
# nnUtils.py this module is pure image processing, so the ~40 exercises that
# only do image processing no longer pay for importing torch.


# The repository root is the parent of the Python folder that holds this file.
_REPO_ROOT = Path(__file__).resolve().parent.parent


def dataDir(*subFolders: Union[str, Path]) -> Path:
    """
    Path to the repository wide data folder for downloaded datasets.

    All datasets that a script downloads, unzips or generates belong in the
    folder ``data`` next to the ``Python`` folder. That folder is git ignored,
    so the several GB of MNIST, CIFAR10, ISIC, CamVid and BraTS data never end
    up in the repository. The images and videos that are part of the course
    material stay where they are and are reached with :func:`repoDir`.

    The path is derived from the location of this file, not from the current
    working directory. A script therefore finds the same data folder whether it
    is started from the Python folder, from a subfolder such as ``mnist`` or
    from an IDE that runs it with the repository root as working directory.

    Parameters
    ----------
    *subFolders : str or pathlib.Path
        Optional subfolders below the data folder, e.g. ``dataDir("ISIC",
        "2019")`` for ``<repo>/data/ISIC/2019``.

    Returns
    -------
    pathlib.Path
        The folder, created together with its parents if it did not exist.
    """
    folder = _REPO_ROOT / "data"
    if subFolders:
        folder = folder.joinpath(*[str(s) for s in subFolders])
    folder.mkdir(parents=True, exist_ok=True)
    return folder


def repoDir(*subPaths: Union[str, Path]) -> Path:
    """
    Path to a file or folder of the course material in the repository.

    The images, videos and OpenCV model files that belong to the course live
    in ``<repo>/Images``, ``<repo>/Videos`` and ``<repo>/opencv``. Written as
    ``"../Images/Lena.jpg"`` they only resolve while the working directory
    happens to be the ``Python`` folder. Started from the repository root,
    from a subfolder such as ``mnist``, or from an IDE that is configured with
    the project root as working directory, the very same script dies with a
    FileNotFoundError on a path that is perfectly correct one folder further
    down. Like :func:`dataDir` this derives the path from the location of this
    file instead, so it is right from every working directory::

        img = utl.imread(utl.repoDir("Images", "Lena.jpg"))
        cap = cv2.VideoCapture(utl.repoDir("Videos", "street.avi"))

    In contrast to :func:`dataDir` nothing is created here. These files are
    part of the repository, so a path that does not exist is a typo and should
    surface as an error instead of as a silently created empty folder.

    Parameters
    ----------
    *subPaths : str or pathlib.Path
        Path elements below the repository root. Slashes inside an element are
        allowed, so ``repoDir("Images/Lena.jpg")`` and ``repoDir("Images",
        "Lena.jpg")`` are the same path. Without arguments the repository root
        itself is returned.

    Returns
    -------
    pathlib.Path
        The path, whether or not it exists. Most of OpenCV takes it as it is:
        every argument that the bindings declare as a filename accepts a Path,
        e.g. cv2.imread, cv2.imwrite, cv2.VideoCapture, cv2.VideoWriter,
        cv2.CascadeClassifier.load, cv2.dnn.readNet* and cv2.FileStorage.

        A few functions declare a plain string instead of a filename and
        refuse a Path with "Can't convert object to 'str'". Wrap those in
        str(), as in the Snapchat exercises::

            face_detector.load(cv.samples.findFile(str(face_cascade_path)))
            facemark.loadModel(str(lbf_model_path))

        The same holds for cv2.quality.QualityBRISQUE_*. Watch out for the
        loadModel one: in the exercises it sits in a try/except that turns the
        TypeError into "Facemark LBF not available", so a forgotten str() does
        not crash, it silently drops the face landmarks.
    """
    if not subPaths:
        return _REPO_ROOT
    return _REPO_ROOT.joinpath(*[str(s) for s in subPaths])


def imread(filename, flags=cv2.IMREAD_COLOR):
    """
    cv2.imread that fails loudly instead of returning None.

    cv2.imread returns None for a missing, misspelled or unreadable file. The
    script then carries on and blows up much later inside an unrelated OpenCV
    call, e.g. with "Unsupported depth" in cvtColor, which says nothing about
    the real cause. Raising here names the file that could not be read.

    Note that on macOS the file system is case insensitive by default, so a
    path like "../images/x.png" works locally but not on Linux, where the
    folder is "../Images". Such a typo surfaces as a FileNotFoundError here.
    """
    img = cv2.imread(str(filename), flags)
    if img is None:
        raise FileNotFoundError(f"Could not read image: {Path(filename)} "
                                f"(resolved: {Path(filename).resolve()})")
    return img


def float01_to_u8(imgf):
    """
    Convert a float image in [0,1] back to uint8 in [0,255].

    The +0.5 rounds to the nearest integer instead of truncating, like
    MATLABs im2uint8 does. Values outside [0,1] are clipped, so this is the
    inverse of im2float/im2double for everything that is displayable.
    """
    return np.clip(imgf * 255.0 + 0.5, 0, 255).astype(np.uint8)


def im2double(img):
    """Convert uint8/uint16/float to float64 in [0,1]."""
    if img.dtype == np.uint8:
        return img.astype(np.float64) / 255.0
    if img.dtype == np.uint16:
        return img.astype(np.float64) / 65535.0
    if img.dtype.kind == "f":
        return np.clip(img.astype(np.float64), 0.0, 1.0)
    return img.astype(np.float64)


def im2float(img):
    """Convert uint8/uint16/float to float32 in [0,1]."""
    if img.dtype == np.uint8:
        return img.astype(np.float32) / 255.0
    if img.dtype == np.uint16:
        return img.astype(np.float32) / 65535.0
    if img.dtype.kind == "f":
        return np.clip(img.astype(np.float32), 0.0, 1.0)
    return img.astype(np.float32)


def std2(img):
    """
    Standard deviation over ALL elements of an image, like MATLABs std2.

    Careful with the scale: this works on the values as they are, so a uint8
    image gives a result in 0..255 units and a float image in [0,1] one in
    0..1 units - a factor of 255, or 65025 once you square it to a variance.
    Parameters like the degreeOfSmoothing of imbilatfilt expect the 8-bit
    scale, so pass the uint8 image there, not the float version.
    """
    return float(np.std(img.astype(np.float64)))


def imnoise(img, kind='gaussian', var=0.01, mean=0.0, density=0.05,
            salt_vs_pepper=0.5):
    """
    Add noise to an image, like MATLABs imnoise.

    Works on uint8 and on float images in [0,1] and returns the same dtype as
    the input; the noise is always computed on the [0,1] scale, as in MATLAB.

    Args:
        kind:           'gaussian' or 'salt & pepper'
        var, mean:      variance/mean of the Gaussian noise, on the [0,1]
                        scale (MATLAB defaults 0.01 and 0)
        density:        fraction of pixels to corrupt for salt & pepper
                        (MATLAB default 0.05)
        salt_vs_pepper: fraction of salt among the corrupted pixels

    Unlike MATLAB, salt & pepper corrupts exactly round(density*N) pixels
    rather than an expected number, and on a colour image it corrupts whole
    pixels (black or white dots) instead of single channels (colour speckle).
    """
    was_u8 = img.dtype == np.uint8
    f = im2float(img) if was_u8 else img.astype(np.float32).copy()

    if kind == 'gaussian':
        noise = np.random.normal(mean, np.sqrt(var),
                                 size=f.shape).astype(np.float32)
        out = np.clip(f + noise, 0.0, 1.0)

    elif kind in ('salt & pepper', 'salt&pepper'):
        out = f.copy()
        h, w = out.shape[:2]
        num = int(round(density * h * w))
        num_salt = int(round(num * salt_vs_pepper))

        idx = np.random.choice(h * w, size=num, replace=False)
        salt_idx, pepper_idx = idx[:num_salt], idx[num_salt:]

        flat = out.reshape(h * w, -1) if out.ndim == 3 else out.reshape(-1)
        if out.ndim == 3:
            flat[salt_idx, :] = 1.0
            flat[pepper_idx, :] = 0.0
        else:
            flat[salt_idx] = 1.0
            flat[pepper_idx] = 0.0
        out = flat.reshape(out.shape)

    else:
        raise ValueError(f"imnoise: unknown kind {kind!r}")

    return float01_to_u8(out) if was_u8 else out


def _imshow(img):
    """
    Internal: plt.imshow with the right colour handling for our images.

    Two things it takes care of, both of which are easy to get wrong:
    - Display range. A float image is assumed to be in [0,1] and a uint8 one
      in [0,255]. Data outside that range is not a displayable image but a
      signed quantity (an eigenvector, a filter response), so BOTH ends of
      the range are freed and matplotlib scales to min..max, like MATLABs
      imshow(img, []).
    - Channel order. OpenCV delivers BGR, matplotlib expects RGB.

    Used by imshow*/subplot; call those rather than this one.
    """
    if img.ndim == 2:
        # Gray scale
        lo, hi = (0, 1) if img.dtype.kind == "f" else (0, 255)
        vmin, vmax = lo, hi
        if img.min() < lo or img.max() > hi :
            # Not a displayable [0,1] resp. [0,255] image (e.g. a signed
            # eigenvector or a filter response). Free BOTH ends so matplotlib
            # scales to min..max, like MATLABs imshow(img, []). Freeing only the
            # exceeded end would squeeze e.g. eigenfaces (values of about
            # +/- 0.03) into the bottom 3% of a 0..1 ramp and show them black.
            vmin, vmax = None, None

        plt.imshow(img, cmap="gray", vmin=vmin, vmax=vmax)
    else:
        # if float image, avoid cv.cvtColor (no float64 support). Just swap channels.
        if img.dtype.kind == "f":
            plt.imshow(img[..., ::-1])  # BGR -> RGB
        else:
            plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))


def imshow(img, title, sizeInInch=(10, 8)):
    """Show one image in its own window. Alias of imshow1x1."""
    imshow1x1(img, title, sizeInInch)


def imshow1x1(img, title, sizeInInch=(10, 8)):
    """
    Show one image in a new figure, without axes, and block until it is
    closed. Colour order and display range are handled by _imshow.

    Args:
        img:        gray or BGR image, uint8 or float
        title:      figure title
        sizeInInch: figure size (width, height) in inches
    """
    plt.figure(figsize=sizeInInch)
    _imshow(img)
    plt.title(title)
    plt.axis('off')
    plt.show()


def imshow3x3(img1, title1, img2, title2, img3, title3, sizeInInch=(12, 5)):
    """
    Show three images side by side, each with its own title.

    Careful with the name: the images are laid out in ONE row of THREE
    columns (1x3), not in a 3x3 grid.
    """
    plt.figure(figsize=sizeInInch)
    plt.subplot(1, 3, 1)
    _imshow(img1)
    plt.title(title1)
    plt.axis('off')
    plt.subplot(1, 3, 2)
    _imshow(img2)
    plt.title(title2)
    plt.axis('off')
    plt.subplot(1, 3, 3)
    _imshow(img3)
    plt.title(title3)
    plt.axis('off')
    plt.tight_layout()
    plt.show()


def imshow2x2(img1, title1, img2, title2, img3, title3, img4, title4, sizeInInch=(12, 12)):
    """Show four images in a 2x2 grid, each with its own title."""
    plt.figure(figsize=sizeInInch)
    plt.subplot(2, 2, 1)
    _imshow(img1)
    plt.title(title1)
    plt.axis('off')
    plt.subplot(2, 2, 2)
    _imshow(img2)
    plt.title(title2)
    plt.axis('off')
    plt.subplot(2, 2, 3)
    _imshow(img3)
    plt.title(title3)
    plt.axis('off')
    plt.subplot(2, 2, 4)
    _imshow(img4)
    plt.title(title4)
    plt.axis('off')
    plt.tight_layout()
    plt.show()


def imhist(img, title, xlabel='Pixel Value', ylabel='Frequency', sizeInInch=(10, 6)):
    """
    Plot the intensity histogram of an image, like MATLABs imhist.

    Uses 256 bins over the fixed range 0..255 and normalizes to a density, so
    histograms of differently sized images are comparable. All channels go
    into one histogram. Note the fixed range: pass a uint8 image, a float
    image in [0,1] would end up entirely in the first bin.
    """
    plt.figure(figsize=sizeInInch)
    plt.hist(img.ravel(), bins=256, range=(0, 255), density=True)
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.show()


def subplot(rows, cols, index, thingToPlot, title, axis="off"):
    """
    Place one image into a cell of a grid in the CURRENT figure.

    Unlike the imshow* helpers this does NOT call plt.show(), so several
    calls build up one figure. Create the figure with plt.figure() first and
    call plt.show() yourself when the grid is complete.

    Args:
        rows, cols:  grid size
        index:       1-based cell number, counted row by row (as in MATLAB)
        thingToPlot: gray or BGR image
        title:       title of this cell
        axis:        "off" (default) or "on" to keep the axes visible
    """
    plt.subplot(rows, cols, index)
    _imshow(thingToPlot)
    plt.title(title)
    plt.axis(axis)
    # don't plt.show()


def imtile(images, rows=None, cols=None, pad=2, bg=255):
    """
    Montage a list of equally sized images into one image, like MATLABs
    imtile. Returns the tiled image, it does NOT plot anything.

    Args:
        images:     list of HxWxC (or HxW) arrays of the same size and dtype
        rows, cols: grid size. Derived from len(images) if omitted, aiming at
                    16 columns.
        pad:        border between the tiles in pixels
        bg:         border colour, 255 = white (0 = black suits filter banks)

    Missing cells at the end of the last row stay in the border colour.
    """
    if len(images) == 0:
        raise ValueError("No images to tile.")
    if cols is None:
        cols = min(16, len(images))
    if rows is None:
        rows = int(np.ceil(len(images) / cols))

    first = images[0]
    H, W = first.shape[:2]
    C = first.shape[2] if first.ndim == 3 else 1
    shape = (rows * H + pad * (rows - 1), cols * W + pad * (cols - 1))
    grid = np.full(shape + ((C,) if first.ndim == 3 else ()), bg,
                   dtype=first.dtype)

    for idx, img in enumerate(images[:rows * cols]):
        r, c = idx // cols, idx % cols
        grid[r * (H + pad):r * (H + pad) + H,
             c * (W + pad):c * (W + pad) + W] = img
    return grid


def show_tiled(title, images, rows=None, cols=None, pad=2, bg=255,
               figsize=(10, 6), min_px=0):
    """
    Tile images with imtile() and show them in a new figure.

    min_px blows small tiles up with nearest neighbour first, so that e.g.
    11x11 conv filters stay recognizable. 0 disables the magnification.
    """
    if min_px and images[0].shape[0] < min_px:
        f = int(np.ceil(min_px / images[0].shape[0]))
        reps = (f, f, 1) if images[0].ndim == 3 else (f, f)
        images = [np.kron(im, np.ones(reps, dtype=im.dtype)) for im in images]

    plt.figure(figsize=figsize)
    plt.imshow(imtile(images, rows, cols, pad, bg))
    plt.axis('off')
    plt.title(title)
    plt.show()


def scatter_positions(pos, color, dx=0.0, dy=0.0, marker='x', label=None):
    """
    Draw markers for the positions returned by np.where() on the current axes.

    np.where gives (rows, cols) while matplotlib wants (x, y), so the two are
    swapped here. dx/dy shift the markers, which is what you need to go from
    the corner of a correlation window to its centre.

    Args:
        pos:    tuple of index arrays, as returned by np.where(condition)
        color:  matplotlib colour of the markers
        dx, dy: offset added to the column resp. row index
    """
    if pos[0].size == 0:
        return
    xs = pos[1].astype(np.float64) + dx
    ys = pos[0].astype(np.float64) + dy
    plt.scatter(xs, ys, marker=marker, s=50, linewidths=2, c=color, label=label)


def plot_edge(pointsNx2, i, j, color, lw=2):
    """Helper to draw an edge between two indexed points."""
    plt.plot([pointsNx2[i, 0], pointsNx2[j, 0]],
             [pointsNx2[i, 1], pointsNx2[j, 1]],
             color=color, linewidth=lw)


def plot_xy_dual(x, y_left, y_right,
                 xlabel, left_ylabel, right_ylabel,
                 title):
    '''
    Plots two curves with the same x values but different y values
    and different labels to the left in blue and the right in red.   
    '''
    fig, ax1 = plt.subplots()
    ax1.plot(x, y_left, "b-o", label=left_ylabel)
    ax1.set_xlabel(xlabel)
    ax1.set_ylabel(left_ylabel, color="b")
    ax1.tick_params(axis='y', labelcolor="b")
    ax1.set_title(title)
    ax1.invert_xaxis()  # match MATLAB's 'XDir','reverse'

    ax2 = ax1.twinx()
    ax2.plot(x, y_right, "r-o", label=right_ylabel)
    ax2.set_ylabel(right_ylabel, color="r")
    ax2.tick_params(axis='y', labelcolor="r")

    fig.tight_layout()
    plt.show()


def download_and_unzip(
    zip_file_url: str,
    download_folder: Union[str, Path],
    extracted_folder_or_filename: str,
) -> Path:
    """
    Download a zip archive from ``zip_file_url`` into ``download_folder`` and
    unzip it.  If the expected extracted object (folder or file) already exists,
    the download/unzip steps are skipped.

    Parameters
    ----------
    zip_file_url : str
        URL that points to the ``.zip`` archive.
    download_folder : str or pathlib.Path
        Directory where the zip file will be stored temporarily and where the
        archive will be extracted.
    extracted_folder_or_filename : str
        Name of the folder or file that the archive is supposed to contain.
        The function returns the full path to that object.

    Returns
    -------
    pathlib.Path
        Path to the extracted folder/file (the first entry in the archive).

    Notes
    -----
    * The function deletes the downloaded ``.zip`` file after extraction.
    * If the extracted name does **not** match ``extracted_folder_or_filename``,
      a warning is printed but the function still returns the actual path.
    * Uses only the Python standard library (no external dependencies).
    """
    # Normalise inputs
    download_folder = Path(download_folder).expanduser().resolve()
    expected_path = download_folder / extracted_folder_or_filename

    # Early‑exit if the expected output already exists
    if expected_path.is_file() or expected_path.is_dir():
        print(
            f"Extracted folder or file exists: {extracted_folder_or_filename}, No download.")
        return expected_path

    # Ensure download folder exists
    download_folder.mkdir(parents=True, exist_ok=True)

    # Build a local filename for the zip file
    # Keep the original filename from the URL (e.g. “…/data.zip” → data.zip)
    parsed = urlparse(zip_file_url)
    zip_name = os.path.basename(parsed.path)          # includes extension
    if not zip_name.lower().endswith('.zip'):
        # safety net – enforce .zip extension if the URL is malformed
        zip_name = f"{Path(zip_name).stem}.zip"
    zip_path = download_folder / zip_name

    # Download the archive
    print(f"Downloading: {zip_file_url} ...")
    # urlretrieve shows a simple progress bar; replace with `requests` if you
    # prefer a richer implementation.
    urlretrieve(zip_file_url, zip_path)

    # Unzip
    print(f"Unzipping: {zip_path} ...")
    with zipfile.ZipFile(zip_path, 'r') as zf:
        # Extract *all* members into the download folder
        zf.extractall(download_folder)

        # `namelist()` returns the paths inside the zip (as stored).  The first
        # entry is taken to mimic MATLAB's `files{1}` behaviour.
        first_member = zf.namelist()[0]
        extracted_path = download_folder / first_member

    # Resolve any possible nested folders (e.g. zip contains a top‑level folder)
    extracted_path = extracted_path.resolve()

    # Verify that the extracted top‑level name matches the expectation
    # The top‑level folder/file is the *parent* of the first extracted entry
    top_level_path = extracted_path.parent if extracted_path.is_file() else extracted_path
    top_level_name = top_level_path.name

    if top_level_name != extracted_folder_or_filename:
        print(f"Extracted folder or file name: {top_level_name}")
        print(f"Expected folder or file name: {extracted_folder_or_filename}")

    # Clean up the zip file
    zip_path.unlink(missing_ok=True)

    # Return the path that the caller expects (or the actual one)
    # If the top‑level name matches the expected one we can safely return it.
    # Otherwise we return the *actual* path so the caller can still use it.
    return expected_path if expected_path.exists() else extracted_path


def download(
    download_file_url: str,
    download_folder: Union[str, Path],
    expected_filename: str = "",
) -> Path:
    """
    Download a single (non zipped) file from ``download_file_url`` into
    ``download_folder``. If the expected file already exists the download is
    skipped. Counterpart of :func:`download_and_unzip` for plain files, e.g.
    the ISIC metadata and ground truth CSVs.

    Parameters
    ----------
    download_file_url : str
        URL that points to the file.
    download_folder : str or pathlib.Path
        Directory the file is stored in.
    expected_filename : str, optional
        Name the file is supposed to have. Defaults to the name in the URL.

    Returns
    -------
    pathlib.Path
        Path to the downloaded file.
    """
    download_folder = Path(download_folder).expanduser().resolve()
    url_name = os.path.basename(urlparse(download_file_url).path)
    expected_path = download_folder / (expected_filename or url_name)

    if expected_path.is_file():
        print(f"File exists: {expected_path.name}, No download.")
        return expected_path

    download_folder.mkdir(parents=True, exist_ok=True)

    print(f"Downloading: {download_file_url} ...")
    downloaded_path = download_folder / url_name
    urlretrieve(download_file_url, downloaded_path)

    if url_name != expected_path.name:
        print(f"Downloaded file name: {url_name}")
        print(f"Expected file name: {expected_path.name}")
        return downloaded_path

    return expected_path


def warpTriangle(img1, img2, tri1, tri2):
    """
    Warps a triangular region from img1 into img2 (in place).

    Args:
        img1: source image
        img2: target image, modified in place
        tri1: list of 3 points (triangle) in the source image
        tri2: list of 3 points (triangle) in the target image
    """
    # Find bounding rectangle for each triangle
    rect1 = cv2.boundingRect(np.array(tri1, dtype=np.float32))
    rect2 = cv2.boundingRect(np.array(tri2, dtype=np.float32))

    # Offset points by left top corner of the respective rectangles
    tri1_cropped = []
    tri2_cropped = []
    tri2_cropped_int = []

    for i in range(3):
        tri1_cropped.append((tri1[i][0] - rect1[0], tri1[i][1] - rect1[1]))
        tri2_cropped.append((tri2[i][0] - rect2[0], tri2[i][1] - rect2[1]))
        tri2_cropped_int.append(
            (int(tri2_cropped[i][0]), int(tri2_cropped[i][1])))

    # Apply warpImage to small rectangular patches
    img1_cropped = img1[rect1[1]:rect1[1] + rect1[3],
                        rect1[0]:rect1[0] + rect1[2]].copy()

    # Given a pair of triangles, find the affine transform
    warp_mat = cv2.getAffineTransform(
        np.array(tri1_cropped, dtype=np.float32),
        np.array(tri2_cropped, dtype=np.float32)
    )

    # Apply the Affine Transform just found to the src image
    img2_cropped = np.zeros((rect2[3], rect2[2], img1_cropped.shape[2]),
                            dtype=img1_cropped.dtype)
    cv2.warpAffine(
        img1_cropped,
        warp_mat,
        (rect2[2], rect2[3]),
        img2_cropped,
        flags=cv2.INTER_LINEAR,
        borderMode=cv2.BORDER_REFLECT_101
    )

    # Create white triangle mask
    mask = np.zeros((rect2[3], rect2[2], 3), dtype=np.float32)
    cv2.fillConvexPoly(
        mask,
        np.array(tri2_cropped_int, dtype=np.int32),
        (1.0, 1.0, 1.0),
        lineType=cv2.LINE_AA
    )

    # Delete all outside of warped triangle
    img2_cropped = img2_cropped * mask

    # Delete all inside the target triangle
    img2_rect = img2[rect2[1]:rect2[1] + rect2[3],
                     rect2[0]:rect2[0] + rect2[2]]
    img2_rect = img2_rect * (1.0 - mask)

    # Add warped triangle to target image
    img2[rect2[1]:rect2[1] + rect2[3],
         rect2[0]:rect2[0] + rect2[2]] = img2_rect + img2_cropped


def normalized_tensor_to_numpy_img(tensorImg, mean, std):
    ''' Images of datasets loaded with a normalization transform must be
        denormalized before visualization. In addition matplotlib.imshow
        wants the dimensions as (Height, Width, Channels) 
    '''
    # Reverse the normalization: original = normalized * std + mean
    for t, m, s in zip(tensorImg, mean, std):
        t.mul_(s).add_(m)

    # From (Channels, Height, Width) to (Height, Width, Channels) for matplot
    npImg = np.transpose(tensorImg.numpy(), (1, 2, 0))

    # Clip to valid range [0, 1] in case of small numerical errors
    npImg = np.clip(npImg, 0, 1)
    return npImg


def seed_all_random_generators(seed: int = 42):
    """
    Seed every random generator that is already imported, so a training run
    is reproducible.

    Covers random, numpy and torch (including CUDA and the cuDNN kernel
    choice). Modules that are not imported yet are skipped, so call this
    AFTER the imports of the script.
    """
    random.seed(seed)

    if "numpy" in sys.modules:
        import numpy as np
        np.random.seed(seed)

    if "torch" in sys.modules:
        import torch
        torch.manual_seed(seed)
        if torch.cuda.is_available():
            torch.cuda.manual_seed_all(seed)
            # Pick the same cuDNN kernels every run instead of benchmarking
            # them, otherwise the results are not bit reproducible.
            torch.backends.cudnn.deterministic = True
            torch.backends.cudnn.benchmark = False

    print(f"Seeded available modules with {seed}.")



/**
 * \file      SLGLUniform.h
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLGLUNIFORM_H
#define SLGLUNIFORM_H

#include <SL.h>
#include <SLEventHandler.h>

//-----------------------------------------------------------------------------
//! Template for a single GLSL uniform variable.
/*! Class for GLSL uniform variables that change per frame. An SLGLProgram
holds a list of this type of uniform variables that are applied within the
beginUse method.
*/
template<class T>
class SLGLUniform : public SLEventHandler
{
public:
    /**
     * @brief Constructs a custom uniform variable
     * @param type Type of uniform variable
     * @param name Name of uniform variable
     * @param value Current value 
     * @param inc Increment value
     * @param min Minimal value
     * @param max Maximal value
     * @param keyInc Key F1-F10 to increment the value decrement with SHIFT key
     */
    SLGLUniform(SLUniformType type,
                const SLchar* name,
                T             value,
                T             inc    = 0.0f,
                T             min    = 0.0f,
                T             max    = 0.0f,
                SLKey         keyInc = K_none)
    {
        _name   = name;
        _value  = value;
        _min    = min;
        _max    = max;
        _inc    = inc;
        _type   = type;
        _keyInc = keyInc;

        if (_keyInc != K_none && !(_keyInc >= K_F1 && _keyInc <= K_F10))
            SL_EXIT_MSG("Please use keys F1-F10 for uniform variables.");
    }

    SLGLUniform(const SLchar*     name,
                function<T(void)> getFunc)
    {
        _name    = name;
        _type    = UT_lambda;
        getValue = getFunc;
    }

    const SLchar* name() { return _name.c_str(); }

    /*!
    calculates the current value & returns it.
    This method is called on every frame.
    */
    T value()
    {
        if (_type == UT_const)
            return _value;

        if (_type == UT_lambda)
            return getValue();

        if (_type == UT_incDec)
        {
            if ((_inc > 0.0f && _value >= _max) ||
                (_inc < 0.0f && _value <= _min)) _inc *= -1;
            _value += _inc;
            return _value;
        }

        if (_type == UT_incInc)
        {
            if (_inc > 0 && _value >= _max) _value = _min;
            if (_inc < 0 && _value <= _min) _value = _max;
            _value += _inc;
            return _value;
        }

        if (_type == UT_inc)
        {
            _value += _inc;
            return _value;
        }

        if (_type == UT_random)
        {
            _value = _min + ((T)rand() / (T)RAND_MAX) * (_max - _min);
            return _value;
        }

        if (_type == UT_seconds)
        {
            _value = (T)clock() / CLOCKS_PER_SEC;
            return _value;
        }
        else
            return _value;
    }

    //! Key press event-handler
    SLbool onKeyPress(const SLKey key, const SLKey mod) override
    {
        if (_keyInc != K_none)
        {
            if (key == _keyInc)
            {
                if (mod == K_none)
                {
                    if (_value < _max)
                    {
                        _value += _inc;
                        SL_LOG("Uniform: %s = %5.4f", _name.c_str(),(float)_value);
                        return true;
                    }
                    else if (_inc == _max) // Toggle between min & max
                    {
                        _value = _min;
                        SL_LOG("Uniform: %s = %5.4f", _name.c_str(),(float)_value);
                        return true;
                    }
                }
                else if (mod == K_shift)
                {
                    if (_value > _min)
                    {
                        _value -= _inc;
                        SL_LOG("Uniform: %s = %5.4f", _name.c_str(),(float)_value);
                        return true;
                    }
                }
            }
        }
        return false;
    }

private:
    SLstring          _name;    //!< Name of the variable
    T                 _value;   //!< Current value
    T                 _max;     //!< Max. value for IncInc, IncDec & random types
    T                 _min;     //!< Min. value for IncInc, IncDec & random types
    T                 _inc;     //!< Increment value for IncInc, IncDec & Inc types
    SLUniformType     _type;    //!< Uniform1f type
    SLKey             _keyInc;  //!< keyboard key incrementing const values
    function<T(void)> getValue; //!< lambda getter function
};
//-----------------------------------------------------------------------------
typedef SLGLUniform<SLfloat> SLGLUniform1f;
typedef SLGLUniform<SLint>   SLGLUniform1i;
typedef SLGLUniform<SLuint>  SLGLUniform1u;
//-----------------------------------------------------------------------------
//! STL vector of SLGLShaderUniform1f pointers
typedef vector<SLGLUniform1f*> SLVUniform1f;
typedef vector<SLGLUniform1i*> SLVUniform1i;
//-----------------------------------------------------------------------------
#endif

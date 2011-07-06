#!/usr/bin/env python
# (C) Copyright 2010 Brandyn A. White
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Helper functions for feature computation
"""

__author__ = 'Brandyn A. White <bwhite@cs.umd.edu>'
__license__ = 'GPL V3'

import Image
import cv
import warnings


def _convert_color(image, code, depth, channels):
    """Convert an OpenCV image's color

    Args:
        image: OpenCV IPLImage or CvMat in RGB or BGR mode
        code: OpenCV color code (e.g., cv.CV_BGR2LAB)
        depth: OpenCV depth (e.g., cv.IPL_DEPTH_32F or cv.IPL_DEPTH_8U)
        channels: Number of channels in the output image

    Returns:
        IPLImage of 'depth' with 'channels' in the color space specified by 'code'
    """
    if image.channels != 3:
        raise ValueError('Image must not be gray')
    if image.depth != cv.IPL_DEPTH_32F and image.depth != cv.IPL_DEPTH_8U:
        raise ValueError('Image must be either 32F or 8U! It is [%s]' % image.depth)
    image_f = image
    if depth == cv.IPL_DEPTH_32F:
        if image.depth == cv.IPL_DEPTH_8U:
            image_f = cv.CreateImage(cv.GetSize(image), depth, 3)
            cv.CvtScale(image, image_f, 1 / 255.)
            image = image_f
    elif depth == cv.IPL_DEPTH_8U:
        if image.depth == cv.IPL_DEPTH_32F:
            image_f = cv.CreateImage(cv.GetSize(image), depth, 3)
            cv.CvtScale(image, image_f, 255.)
            image = image_f
    image_convert = cv.CreateImage(cv.GetSize(image_f), depth, channels)
    cv.CvtColor(image_f, image_convert, code)
    return image_convert


def _convert_depth(image, depth):
    """Convert an OpenCV image's color

    Args:
        image: OpenCV IPLImage or CvMat
        depth: OpenCV depth (e.g., cv.IPL_DEPTH_32F or cv.IPL_DEPTH_8U)

    Returns:
        IPLImage of 'depth' with 3 channels in the color space specified by 'code'
    """
    image_f = image
    if depth == cv.IPL_DEPTH_32F:
        if image.depth == cv.IPL_DEPTH_8U:
            image_f = cv.CreateImage(cv.GetSize(image), depth, image.channels)
            cv.CvtScale(image, image_f, 1 / 255.)
            image = image_f
    elif depth == cv.IPL_DEPTH_8U:  # TODO Test to see if these conversions are correct
        if image.depth == cv.IPL_DEPTH_32F:
            image_f = cv.CreateImage(cv.GetSize(image), depth, image.channels)
            cv.CvtScale(image, image_f, 255.)
            image = image_f
    return image_f


def _convert_cv_bgr(image, mode, depth):
    """Convert an OpenCV image's color

    Args:
        image: OpenCV IPLImage or CvMat
        code: OpenCV color code (e.g., cv.CV_BGR2LAB)
        depth: OpenCV depth (e.g., cv.IPL_DEPTH_32F or cv.IPL_DEPTH_8U)

    Returns:
        IPLImage of 'depth' with 3 channels in the color space specified by 'code'
    """
    mode = mode.lower()
    if mode == 'bgr':
        return _convert_depth(image, depth)
    code = {'rgb': cv.CV_BGR2RGB,
            'gray': cv.CV_BGR2GRAY,
            'hls': cv.CV_BGR2HLS,
            'hsv': cv.CV_BGR2HSV,
            'lab': cv.CV_BGR2Lab,
            'luv': cv.CV_BGR2Luv,
            'xyz': cv.CV_BGR2XYZ,
            'ycrcb': cv.CV_BGR2YCrCb}[mode]
    channels = 3 if mode != 'gray' else 1
    return _convert_color(image, code, depth, channels)


def _convert_cv_rgb(image, mode, depth):
    """Convert an OpenCV image's color

    Args:
        image: OpenCV IPLImage or CvMat
        code: OpenCV color code (e.g., cv.CV_RGB2LAB)
        depth: OpenCV depth (e.g., cv.IPL_DEPTH_32F or cv.IPL_DEPTH_8U)

    Returns:
        IPLImage of 'depth' with 3 channels in the color space specified by 'code'
    """
    mode = mode.lower()
    if mode == 'rgb':
        return _convert_depth(image, depth)
    code = {'bgr': cv.CV_RGB2BGR,
            'gray': cv.CV_RGB2GRAY,
            'hls': cv.CV_RGB2HLS,
            'hsv': cv.CV_RGB2HSV,
            'lab': cv.CV_RGB2Lab,
            'luv': cv.CV_RGB2Luv,
            'xyz': cv.CV_RGB2XYZ,
            'ycrcb': cv.CV_RGB2YCrCb}[mode]
    channels = 3 if mode != 'gray' else 1
    return _convert_color(image, code, depth, channels)


def _convert_pil(image, mode):
    if image.mode == 'RGBA':
        image = image.convert('RGB')
    if image.mode == 'L':
        if not isinstance(mode, str):
            if mode == ('opencv', 'gray', cv.IPL_DEPTH_8U):
                cv_im = cv.CreateImageHeader(image.size, cv.IPL_DEPTH_8U, 1)
                cv.SetData(cv_im, image.tostring())
                return cv_im
        # At this point it must be that we want a color image
        image = image.convert('RGB')
    if isinstance(mode, str):  # TO PIL
        return image.convert(mode)
    elif mode[0] == 'opencv':
        cv_im = cv.CreateImageHeader(image.size, cv.IPL_DEPTH_8U, 3)
        cv.SetData(cv_im, image.tostring())
        return _convert_cv_rgb(cv_im, mode[1], mode[2])
    else:
        raise ValueError('Mode is not valid! [%s]' % str(mode))


def _convert_cv(image, mode):
    if image.channels != 3:
        if isinstance(mode, str) and mode == 'L':  # TO PIL
            return Image.fromstring("L", cv.GetSize(image),
                                    image.tostring())
        # At this point it must be that we want a color image
        image_convert = cv.CreateImage(cv.GetSize(image), image.depth, 3)
        cv.CvtColor(image, image_convert, cv.CV_GRAY2BGR)
        image = image_convert
    # At this point we assume that the CV image is in BGR format
    if isinstance(mode, str):  # TO PIL
        image = Image.fromstring("RGB", cv.GetSize(image),
                                _convert_cv_bgr(image, 'rgb',
                                                cv.IPL_DEPTH_8U).tostring())
        return image.convert(mode)
    elif mode[0] == 'opencv':
        return _convert_cv_bgr(image, mode[1], mode[2])
    else:
        raise ValueError('Mode is not valid! [%s]' % str(mode))


def convert_image(image, modes):
    """
    Args:
        image: A PIL image or an OpenCV BGR/Gray image (8 bits per channel)
        modes: List of valid image types

    Returns:
        Valid image

    Raises:
        ValueError: There was a problem converting the color.
    """
    if isinstance(image, cv.cvmat):
        image = cv.GetImage(image)
    if Image.isImageType(image) and image.mode == 'LA':
        image = image.convert('L')
    if Image.isImageType(image) and image.mode not in ('L', 'RGB'):
        image = image.convert('RGB')
    if Image.isImageType(image) and (image.mode == 'L' or image.mode == 'RGB'):
        if image.mode not in modes:
            image = _convert_pil(image, modes[0])
    elif isinstance(image, cv.iplimage) and (image.channels == 1 or image.channels == 3) and image.depth == cv.IPL_DEPTH_8U:
        mode = 'bgr' if image.channels == 3 else 'gray'
        if ('opencv', mode, cv.IPL_DEPTH_8U) not in modes:
            image = _convert_cv(image, modes[0])
    elif isinstance(image, cv.iplimage) and (image.channels == 1 or image.channels == 3) and image.depth == cv.IPL_DEPTH_32F:
        mode = 'bgr' if image.channels == 3 else 'gray'
        if ('opencv', mode, cv.IPL_DEPTH_32F) not in modes:
            # Convert to 8bit to bgr
            image = _convert_depth(image, cv.IPL_DEPTH_8U)
            image = _convert_cv(image, modes[0])
    else:
        if Image.isImageType(image):
            raise ValueError('Unknown image type PIL Mode[%s]' % image.mode)
        else:
            raise ValueError('Unknown image type[%s]' % repr(image))
    return image


def compute(feature_module, image, *args, **kw):
    """Compute features while performing conversions and checks.

    Args:
        feature_module: A module that has a make_features function.
        image: A PIL image or an OpenCV BGR/Gray image (8 bits per channel)
        args: Optional positional arguments to be passed on.
        kw: Optional keyword arguments to be passed on.

    Returns:
        Feature computation output.

    Raises:
        ValueError: There was a problem converting the color.
    """
    msg = ('imfeat.compute(feat, image) is deprecated, use feat(image) instead.'
           ' If you wrote the feature, ensure that the baseclass is '
           'imfeat.BaseFeature and _not_ object.')
    warnings.warn(msg, DeprecationWarning, stacklevel=2)
    try:
        modes = feature_module.MODES
    except AttributeError:
        pass
    else:
        image = convert_image(image, modes)
    return feature_module.make_features(image, *args, **kw)


def compute_points(feature_module, image, *args, **kw):
    """Compute feature points while performing conversions and checks.

    Args:
        feature_module: A module that has a make_features function.
        image: A PIL image.
        args: Optional positional arguments to be passed on.
        kw: Optional keyword arguments to be passed on.

    Returns:
        Feature computation output.
    """

    try:
        if image.mode not in feature_module.MODES:
            image = image.convert(feature_module.MODES[0])
    except AttributeError:
        pass
    return feature_module.make_points(image, *args, **kw)

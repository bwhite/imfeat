import numpy as np
cimport numpy as np
import cv
import cv2
from PIL import Image
import warnings
import cStringIO as StringIO
    

def get_new_mode(old_mode):
    out = None
    try:
        if isinstance(old_mode, str):
            out = {'type': 'pil', 'mode': {'l': 'gray', 'rgb': 'rgb'}[old_mode.lower()], 'dtype': 'uint8'}
        elif old_mode[0] == 'opencv':
            out =  {'type': 'opencv', 'mode': old_mode[1], 'dtype': {8: 'uint8', 32: 'float32'}[old_mode[2]]}
    except KeyError:
        pass
    if out:
        warnings.warn("Old Style Mode [%s] deprecated, use [%s]" % (repr(old_mode), repr(out)), DeprecationWarning)
        return out
    raise ValueError('Unknown mode [%s]' % str(old_mode))


def image_to_mode(image):
    # Need to do this initial conversion for cvmat's
    if isinstance(image, cv.cvmat):
        image = cv.GetImage(image)
    if Image.isImageType(image):
        if image.mode.lower() in ('l', 'rgb', 'rgba'):
            return {'type': 'pil', 'mode': {'l': 'gray', 'rgb': 'rgb', 'rgba': 'rgb'}[image.mode.lower()], 'dtype': 'uint8'}
        else:
            raise ValueError('Unknown image type: PIL Mode[%s] (supported: L, RGB)' % image.mode)
    elif isinstance(image, cv.iplimage):
        try:
            return {'type': 'opencv', 'mode': {1: 'gray', 3: 'bgr'}[image.channels], 'dtype': {8: 'uint8', 32: 'float32'}[image.depth]}
        except KeyError:
            pass
        raise ValueError('Unknown image type: OpenCV Channels[%s] (supported: 1, 3) Depth[%s] (supported: 8, 32)' % (image.channels, image.depth))
    elif isinstance(image, np.ndarray):
        if image.dtype.name in ('float32', 'uint8'):
            if image.ndim == 2:
                return {'type': 'numpy', 'dtype': image.dtype.name, 'mode': 'gray'}
            elif image.ndim == 3 and image.shape[2] == 3:
                return {'type': 'numpy', 'dtype': image.dtype.name, 'mode': 'bgr'}
        raise ValueError('Unknown image type: Numpy Shape[%s] (supported: 2 dim, 3 dim with 3 channels) Dtype[%s] (supported: float32, uint8)' % (str(image.shape), image.dtype.name))
    raise ValueError('Unknown image type: [%s]' % str(type(image)))


COLORS = {('bgr', 'rgb'): cv.CV_BGR2RGB,
          ('bgr', 'gray'): cv.CV_BGR2GRAY,
          ('bgr', 'hls'): cv.CV_BGR2HLS,
          ('bgr', 'hsv'): cv.CV_BGR2HSV,
          ('bgr', 'lab'): cv.CV_BGR2Lab,
          ('bgr', 'luv'): cv.CV_BGR2Luv,
          ('bgr', 'xyz'): cv.CV_BGR2XYZ,
          ('bgr', 'ycrcb'): cv.CV_BGR2YCrCb,
          ('rgb', 'bgr'): cv.CV_RGB2BGR,
          ('rgb', 'gray'): cv.CV_RGB2GRAY,
          ('rgb', 'hls'): cv.CV_RGB2HLS,
          ('rgb', 'hsv'): cv.CV_RGB2HSV,
          ('rgb', 'lab'): cv.CV_RGB2Lab,
          ('rgb', 'luv'): cv.CV_RGB2Luv,
          ('rgb', 'xyz'): cv.CV_RGB2XYZ,
          ('rgb', 'ycrcb'): cv.CV_RGB2YCrCb}

def convert_image(image, mode_or_modes, image_mode=None):
    """Convert image to the specified mode if necessary

    If mode_or_modes is a single mode, then it is replaced with
    mode_or_modes = [mode_or_modes]

    If the image mode is present in the 'modes' list, it is returned.  Else
    it is converted using OpenCV's color conversion functions.

    The color channels assumed for 3 channel inputs are
    opencv: 'bgr'
    numpy: 'bgr'
    pil: 'rgb'

    :param image: A PIL image or an OpenCV BGR/Gray image (8 bits per channel)
    :param mode_or_modes: List of image modes or a single image mode.  A mode is
        a dict {'type': type, 'dtype': dtype, 'mode': mode}
        type: Valid options are 'opencv', 'numpy', 'pil'
        dtype: Valid options are 'uint8', 'float32'
        mode: Valid options are 'gray', 'rgb', 'bgr', 'hls', 'hsv',
        'lab', 'luv', 'xyz', 'ycrcb'
    :param image_mode: Mode corresponding to the input image, if None then
        the depth and type will be detected from the input and the depth
        will be assumed to be the default color for the type (see above).
        This is useful if you want to override the default color.


    :returns: Valid image
    :raises: ValueError: There was a problem converting.
    """
    if isinstance(mode_or_modes, dict):
        modes = [mode_or_modes]
    else:
        modes = mode_or_modes
    assert isinstance(modes[0], dict)
    # Convenience conversions:  These are only supported on input so they can't match our target conversion
    if isinstance(image, cv.cvmat):
        image = cv.GetImage(image)
    if Image.isImageType(image) and image.mode == 'LA':
        image = image.convert('L')
    if Image.isImageType(image) and image.mode not in ('L', 'RGB'):
        image = image.convert('RGB')
    # Perform the primary conversions
    if image_mode is None:
        image_mode = image_to_mode(image)
    if image_mode in modes:
        return image
    # Anything that needs a real conversion is first converted to the
    # compatible numpy array and then cv2 is used to change the color space.
    # This prevents differences in color conversion from changing the results.
    if image_mode['type'] == 'pil':
        image = np.array(image)
        image_mode['type'] = 'numpy'
    elif image_mode['type'] == 'opencv':
        image = np.asarray(cv.GetMat(image))
        image_mode['type'] = 'numpy'
    image = np.ascontiguousarray(image)
    out_mode = modes[0]
    # Convert to the correct output data type
    if image_mode['dtype'] == 'float32' and out_mode['dtype'] == 'uint8':
        image = np.asarray(image * 255, dtype=np.uint8)
    elif image_mode['dtype'] == 'uint8' and out_mode['dtype'] == 'float32':
        image = np.asarray(image, dtype=np.float32) * (1. / 255.)
    image_mode['dtype'] = out_mode['dtype']
    # Convert to the correct output color mode
    if image_mode['mode'] != out_mode['mode']:
        if image_mode['mode'] == 'gray':
            image = cv2.cvtColor(image, cv.CV_GRAY2BGR)
            image_mode['mode'] = 'bgr'
        if image_mode['mode'] != out_mode['mode']:
            image = cv2.cvtColor(image, COLORS[(image_mode['mode'], out_mode['mode'])])
            image_mode['mode'] = out_mode['mode']
    # Convert to the correct output type
    if image_mode['type'] != out_mode['type']:
        if out_mode['type'] == 'pil':
            image = Image.fromarray(image)
            image_mode['type'] = 'pil'
        elif out_mode['type'] == 'opencv':
            image = cv.GetImage(cv.fromarray(np.ascontiguousarray(image)))
            image_mode['type'] = 'opencv'
    try:
        assert image_mode == out_mode
    except:
        print('ImageMode[%s] != OutMode[%s]' % (str(image_mode), str(out_mode)))
        raise
    return image


def resize_image(image, height, width=None, image_mode=None):
    """Resize image to a specified size, crop excess

    :param image: Input image
    :param height: Desired image height
    :param width: Desired image width
    :param image_mode: Mode corresponding to the input image, if None then
        the depth and type will be detected from the input and the depth
        will be assumed to be the default color for the type (see above).
        This is useful if you want to override the default color.

    :returns: Resized image in the original image format
    """
    if width is None:
        width = height
    if image_mode is None:
        image_mode = image_to_mode(image)
    temp_mode = dict(image_mode)
    temp_mode['type'] = 'numpy'
    image = convert_image(image, temp_mode)
    cur_height, cur_width = image.shape[:2]
    height_ratio = height / float(cur_height)
    width_ratio = width / float(cur_width)
    interpolation = cv2.INTER_LINEAR
    if width_ratio < .5:
        interpolation = cv2.INTER_AREA
    # The larger ratio is the side that will get met exactly
    if width_ratio > height_ratio:
        new_height = int(np.round(cur_height * width_ratio))
        image = cv2.resize(image, (width, new_height), interpolation=interpolation)
        height_offset = (new_height - height) / 2
        image = image[height_offset:height_offset + height]
    else:
        new_width = int(np.round(cur_width * height_ratio))
        image = cv2.resize(image, (new_width, height), interpolation=interpolation)
        width_offset = (new_width - width) / 2
        image = image[:, width_offset:width_offset + width]
    return convert_image(image, image_mode, image_mode=temp_mode)


def resize_image_max_side(image, max_side, image_mode=None):
    """Resize image to a specified size

    :param image: Input image
    :param max_side: Maximium image side length
    :param image_mode: Mode corresponding to the input image, if None then
        the depth and type will be detected from the input and the depth
        will be assumed to be the default color for the type (see above).
        This is useful if you want to override the default color.

    :returns: Resized image in the original image format
    """
    if image_mode is None:
        image_mode = image_to_mode(image)
    temp_mode = dict(image_mode)
    temp_mode['type'] = 'numpy'
    image = convert_image(image, temp_mode)
    cur_height, cur_width = image.shape[:2]
    size_ratio = max_side / float(max(cur_width, cur_height))
    interpolation = cv2.INTER_LINEAR
    if size_ratio < .5:
        interpolation = cv2.INTER_AREA
    if cur_height > cur_width:
        height = max_side
        width = int(np.round((float(cur_width) / cur_height) * max_side))
    else:
        width = max_side
        height = int(np.round((float(cur_height) / cur_width) * max_side))
    image = cv2.resize(image, (width, height))
    return convert_image(image, image_mode, image_mode=temp_mode)


def image_fromstring(image_data, mode_or_modes=None):
    """Convert an image from a string (using cv2's image IO)

    :param: image_data: Binary image data
    :param: mode_or_modes: List of image modes or a single image mode.  A mode is
        a dict {'type': type, 'dtype': dtype, 'mode': mode}. If None
        (default) then any output type is acceptable (most natural is used).
        type: Valid options are 'opencv', 'numpy', 'pil'
        dtype: Valid options are 'uint8', 'float32'
        mode: Valid options are 'gray', 'rgb', 'bgr', 'hls', 'hsv',
        'lab', 'luv', 'xyz', 'ycrcb'.

    :returns: String of binary image data
    """
    if mode_or_modes is None:
        mode_or_modes = {'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'}
    image = cv2.imdecode(np.frombuffer(image_data, dtype=np.uint8), 1)
    if image is None:  # Try using PIL
        image = np.asarray(Image.open(StringIO.StringIO(image_data)).convert('RGB'))
        image = np.ascontiguousarray(image[:, :, ::-1])
    return convert_image(image, mode_or_modes)


def image_tostring(image, format, image_mode=None):
    """Convert image to a string (using cv2's image IO)

    :param: image: Input image
    :param format: PIL image format to convert to (e.g., 'JPEG', 'PNG')
    :param image_mode: Mode corresponding to the input image, if None then
        the depth and type will be detected from the input and the depth
        will be assumed to be the default color for the type (see above).
        This is useful if you want to override the default color.

    :returns: String of binary image data
    """
    if image_to_mode(image)['mode'] == 'gray':
        image = convert_image(image, {'type': 'numpy', 'dtype': 'uint8', 'mode': 'gray'}, image_mode=image_mode)
    else:
        image = convert_image(image, {'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'}, image_mode=image_mode)
    format = format.upper()
    if format in ('.JPEG', '.JPG', 'JPG', 'JPEG'):
        format = '.jpg'
    elif format in ('.PNG', 'PNG'):
        format = '.png'
    else:
        raise ValueError('Unknown format [%s]' % format)
    # NOTE(brandyn): This uses cv.EncodeImage as cv2.imencode is broken
    image_mat = cv.fromarray(np.ascontiguousarray(image))
    data_out = cv.EncodeImage(format, image_mat)
    return np.asarray(data_out).tostring()

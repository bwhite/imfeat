import imfeat
import numpy as np


def call_import(import_data):
    """
    Args:
        import_data: Dict
            name: Import statement for function/class
            args: Positional arguments to call function with (default [])
            kw: Keyword arguments to pass (default {})
                                                                                                                                            
    Returns:
        Result
    """
    name, attr = import_data['name'].rsplit('.', 1)
    # NOTE(brandyn): the 'arg' makes it give us the most specific module
    m = __import__(name, fromlist=['arg'])
    f = getattr(m, attr)
    return f(*import_data.get('args', []), **import_data.get('kw', {}))


class MetaFeature(imfeat.BaseFeature):

    def __init__(self, *features, **kw):
        super(MetaFeature, self).__init__()
        self._features = [call_import(f) if isinstance(f, dict) else f
                          for f in features]
        self.norm = kw.get('norm', None)
        self.max_side = kw.get('max_side', None)

    def __call__(self, image):
        if self.max_side is not None:
            image = imfeat.resize_image_max_side(image, self.max_side)
        if self.norm is None:
            norm = lambda x: x
        elif norm == 'dims':
            norm = lambda x: x / float(x.size)
        elif norm == 'l1':
            norm = lambda x: x / np.sum(x)
        elif norm == 'l2':
            norm = lambda x: x / np.linalg.norm(x)
        else:
            raise ValueError('Unknown value for norm=%s' % norm)
        return np.hstack([self._norm(f(image)) for f in self._features])

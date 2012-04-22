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
        norm = kw.get('norm', None)
        if norm is None:
            self._norm = lambda x: x
        elif norm == 'dims':
            self._norm = lambda x: x / float(x.size)
        else:
            raise ValueError('Unknown value for norm=%s' % norm)

    def make_features(self, image):
        return [np.hstack([self._norm(imfeat.compute(f, image)[0]) for f in self._features])]

import base64
import cPickle

def size_of_dims(dims):
    ret = 1
    for d in dims:
        ret *= d
    return ret

class TestResult(object):
    def __init__(self, test_name, function_name):
        self.serie_name = "serie name not implemented"
        self.test_name = test_name
        self.function_name = function_name
        self.data = {}

    def add_case(self, n, seed, typ, time, dims = [], size = -1):
        if size == -1:
            size = size_of_dims(dims)
        if typ not in self.data:
            self.data[typ] = []
        self.data[typ].append((n, size, dims, seed, time))

    def get_types(self):
        return self.data.keys()

    def get_sorted_sample(self, typ):
        sample = self.data[typ]
        ret = [(a[1], a[4], a[0]) for a in sample]
        #ret = [(size_of_dims(a[1]), a[3], a[0]) for a in sample]
        ret.sort()
        return ret

    def get_encoded(self):
        return base64.b64encode(cPickle.dumps(self)) + "\n"

def decode_result(result):
    return cPickle.loads(base64.b64decode(result))

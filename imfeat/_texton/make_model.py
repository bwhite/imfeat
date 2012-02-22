import zlib
import base64
import cPickle as pickle
import argparse


def models_to_py(output_path, paths):
    data = [(x, open(x).read()) for x in paths]
    with open(output_path, 'w') as fp:
        fp.write('import zlib, base64, cPickle\ndata = cPickle.loads(zlib.decompress(base64.b64decode("')
        fp.write(base64.b64encode(zlib.compress(pickle.dumps(data, -1))))
        fp.write('")))\n')


def main():
    parser = argparse.ArgumentParser(description='Make a .py file that containts the data in the specified files')
    parser.add_argument('output_path', help='Output path')
    parser.add_argument('paths', nargs='+', help='Paths to load')
    args = parser.parse_args()
    models_to_py(args.output_path, args.paths)
    
if __name__ == '__main__':
    main()

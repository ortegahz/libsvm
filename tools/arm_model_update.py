import argparse
import logging


def set_logging():
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)


def run(args):
    logging.info(args)
    db = dict()
    with open(args.path_in_log, 'r') as f:
        lines = f.readlines()
    for i, line in enumerate(lines):
        if args.parer_head not in line:
            continue
        logging.info(line)
        _, _key, _len = line.strip().split(',')
        _len = int(_len)
        data = lines[i + 1]
        # logging.info(data)
        data_lst = data.strip().split(',')
        _size = int(len(data_lst) / _len)
        _key = _key[:-3] if 'sv_coef' in _key else _key
        logging.info((_key, _len, _size))
        db[_key] = (data, _len, _size)
    with open(args.path_in_head, 'r') as f:
        lines = f.readlines()
    with open(args.path_out_head, 'w') as f:
        for line in lines:
            if '_hex' not in line:
                f.write(line)
                continue
            for key in db.keys():
                key_hex = key + '_hex'
                if key_hex not in line:
                    continue
                data_str, _len, _size = db[key][0].strip()[:-1], db[key][1], db[key][2]
                line = f'const unsigned char {key_hex}[{_len * _size}] __attribute__((aligned(16))) = {{{data_str}}};\n'
                f.write(line)



def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--path_in_log', default='/home/manu/tmp/log.txt')
    parser.add_argument('--path_in_head', default='/home/manu/tmp/data.h')
    parser.add_argument('-path_out_head', default='/home/manu/tmp/data_new.h')
    parser.add_argument('--parer_head', default='<python parser mark>')
    return parser.parse_args()


def main():
    set_logging()
    args = parse_args()
    logging.info(args)
    run(args)


if __name__ == '__main__':
    main()

import json
import numpy as np


def main():
    f = 'data/eval.bin.meta.json'
    data = json.load(open(f))
    print(json.dumps(data, indent=4, sort_keys=True))
    num_ticks = data['ticks_per_predict']
    num_classes = data['y_size']

    f = 'data/eval.bin'
    data = np.fromfile(f, 'float32')
    print('Orig shape:', data.shape)
    data = data.reshape((-1, (1 + num_ticks + num_ticks), num_classes))
    print('Reshaped:', data.shape)
    print()

    true_weights = data[:, 0, :]
    true_classes = true_weights.argmax(1)
    print('true classes:', true_classes.shape)
    print()

    x = data[:, num_ticks + 1:, :]
    print('prediction stds across repeats per tick')
    print(x.shape, x.mean(), x.std(), x.min(), x.max())
    print()

    x = data[:, 1:num_ticks + 1, :]
    print('prediction means across repeats per tick')
    print(x.shape, x.mean(), x.std(), x.min(), x.max())
    print()

    x = x.sum(1)
    print('prediction means across repeats (summed across ticks)')
    print(x.shape, x.mean(), x.std(), x.min(), x.max())
    print()

    preds_per_sample = x
    results = []
    for i in range(len(preds_per_sample)):
        true_class = true_classes[i]
        preds = preds_per_sample[i]
        pred_class = preds.argmax()
        min_pred = preds.min()
        xx = preds - min_pred
        xx = np.log(xx)
        xx = np.clip(xx, 1e-3, 1 - 1e-3)
        loss = -xx[true_class]
        results.append((true_class, pred_class, loss))

    for i, (true, pred, loss) in enumerate(results):
        print('%4d true %d pred %d acc %d loss %.3f' %
              (i, true, pred, true == pred, loss))


if __name__ == '__main__':
    main()

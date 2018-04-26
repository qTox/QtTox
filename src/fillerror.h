#ifndef _QT_TOX_FILL_ERROR_H_
#define _QT_TOX_FILL_ERROR_H_

template<class ToxErr, class Err>
void fillError(ToxErr toxErr, Err* err, const QMap<ToxErr, Err>& map)
{
    if (!err) {
        return;
    }

    *err = map[toxErr];
}

#endif // _QT_TOX_FILL_ERROR_H_

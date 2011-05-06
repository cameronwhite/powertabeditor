#ifndef RHYTHMSLASHPAINTER_H
#define RHYTHMSLASHPAINTER_H

#include "painterbase.h"
#include <QFont>
#include <memory>

class RhythmSlash;

class RhythmSlashPainter : public PainterBase
{
public:
    RhythmSlashPainter(std::shared_ptr<const RhythmSlash> rhythmSlash);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    static const double STEM_OFFSET = 3.0;
    static const double NOTE_HEAD_OFFSET;

private:
    std::shared_ptr<const RhythmSlash> rhythmSlash;
    static QFont musicFont;
};

#endif // RHYTHMSLASHPAINTER_H

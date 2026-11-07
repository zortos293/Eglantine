// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <cmath>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QSvgRenderer>
#include <QStringList>
#include "game_card.h"
#include "qt_common/config/uisettings.h"
#include "qt_common/game_list/game_list_p.h"
#include "qt_common/game_list/model.h"

namespace {

void DrawSwitch2Placeholder(QPainter* painter, const QRect& bounds) {
    static QSvgRenderer switch2_logo(QStringLiteral(":/img/switch2_logo.svg"));
    if (switch2_logo.isValid()) {
        painter->fillRect(bounds, QColor(230, 0, 18));
        const int side = std::min(bounds.width(), bounds.height());
        const QRectF logo_rect(bounds.center().x() - side / 2.0,
                               bounds.center().y() - side / 2.0, side, side);
        switch2_logo.render(painter, logo_rect);
        return;
    }

    const QRectF rect(bounds);
    const QColor red(226, 0, 20);
    painter->fillRect(rect, red);

    const qreal width = rect.width();
    const qreal height = rect.height();
    const qreal left = rect.left();
    const qreal top = rect.top();
    const qreal body_width = width * 0.15;
    const qreal body_height = height * 0.39;
    const qreal body_top = top + height * 0.14;
    const qreal inner = std::max(2.0, width * 0.012);

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    const QRectF left_controller(left + width * 0.09, body_top, body_width, body_height);
    const QRectF right_controller(left + width * 0.28, body_top, body_width, body_height);
    painter->drawRoundedRect(left_controller, width * 0.035, width * 0.035);
    painter->drawRoundedRect(right_controller, width * 0.035, width * 0.035);

    painter->setBrush(red);
    painter->drawRoundedRect(left_controller.adjusted(inner, inner, -inner, -inner),
                             width * 0.025, width * 0.025);
    painter->drawRoundedRect(right_controller.adjusted(inner, inner, -inner, -inner),
                             width * 0.025, width * 0.025);

    painter->setBrush(Qt::white);
    const qreal button_size = width * 0.035;
    painter->drawEllipse(QRectF(left_controller.center().x() - button_size / 2,
                                body_top + body_height * 0.30, button_size, button_size));
    painter->drawEllipse(QRectF(right_controller.center().x() - button_size / 2,
                                body_top + body_height * 0.58, button_size, button_size));

    QFont number_font(QStringLiteral("Arial"));
    number_font.setPixelSize(std::max(18, static_cast<int>(height * 0.48)));
    number_font.setWeight(QFont::Black);
    painter->setFont(number_font);
    painter->drawText(QRectF(left + width * 0.47, top + height * 0.04, width * 0.45,
                             height * 0.53),
                      Qt::AlignCenter, QStringLiteral("2"));

    QFont brand_font(QStringLiteral("Arial"));
    brand_font.setPixelSize(std::max(8, static_cast<int>(height * 0.085)));
    brand_font.setWeight(QFont::DemiBold);
    brand_font.setLetterSpacing(QFont::AbsoluteSpacing, std::max(1.0, width * 0.008));
    painter->setFont(brand_font);
    painter->drawText(QRectF(left + width * 0.08, top + height * 0.62, width * 0.84,
                             height * 0.14),
                      Qt::AlignCenter, QStringLiteral("NINTENDO"));

    QFont switch_font(QStringLiteral("Arial"));
    switch_font.setPixelSize(std::max(12, static_cast<int>(height * 0.16)));
    switch_font.setWeight(QFont::Bold);
    painter->setFont(switch_font);
    painter->drawText(QRectF(left + width * 0.08, top + height * 0.74, width * 0.84,
                             height * 0.21),
                      Qt::AlignCenter, QStringLiteral("SWITCH"));
}

void DrawAddFolderArtwork(QPainter* painter, const QRect& bounds, const QColor& accent, bool dark) {
    const QRectF rect(bounds);
    const QColor surface = dark ? QColor(17, 38, 54) : QColor(237, 243, 249);
    painter->fillRect(rect, surface);

    const qreal width = rect.width();
    const qreal height = rect.height();
    const QRectF folder(rect.left() + width * 0.27, rect.top() + height * 0.31,
                        width * 0.46, height * 0.4);

    QPainterPath folder_path;
    folder_path.moveTo(folder.left(), folder.top() + folder.height() * 0.22);
    folder_path.lineTo(folder.left(), folder.top() + folder.height() * 0.08);
    folder_path.quadTo(folder.left(), folder.top(), folder.left() + folder.width() * 0.12,
                       folder.top());
    folder_path.lineTo(folder.left() + folder.width() * 0.4, folder.top());
    folder_path.lineTo(folder.left() + folder.width() * 0.53,
                       folder.top() + folder.height() * 0.2);
    folder_path.lineTo(folder.right() - folder.width() * 0.1,
                       folder.top() + folder.height() * 0.2);
    folder_path.quadTo(folder.right(), folder.top() + folder.height() * 0.2, folder.right(),
                       folder.top() + folder.height() * 0.3);
    folder_path.lineTo(folder.right(), folder.bottom() - folder.height() * 0.1);
    folder_path.quadTo(folder.right(), folder.bottom(), folder.right() - folder.width() * 0.1,
                       folder.bottom());
    folder_path.lineTo(folder.left() + folder.width() * 0.1, folder.bottom());
    folder_path.quadTo(folder.left(), folder.bottom(), folder.left(),
                       folder.bottom() - folder.height() * 0.1);
    folder_path.closeSubpath();

    QColor folder_fill = accent;
    folder_fill.setAlpha(dark ? 42 : 30);
    painter->setPen(QPen(accent, std::max(3.0, std::min(width, height) * 0.018), Qt::SolidLine,
                         Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(folder_fill);
    painter->drawPath(folder_path);

    const qreal badge_size = std::min(width, height) * 0.22;
    const QPointF badge_center(folder.right() - badge_size * 0.05,
                               folder.bottom() - badge_size * 0.04);
    const QRectF badge(badge_center.x() - badge_size / 2, badge_center.y() - badge_size / 2,
                       badge_size, badge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(accent);
    painter->drawEllipse(badge);

    const qreal plus_half = badge_size * 0.22;
    painter->setPen(QPen(dark ? QColor(12, 29, 43) : QColor(Qt::white), badge_size * 0.09,
                         Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(QPointF(badge_center.x() - plus_half, badge_center.y()),
                      QPointF(badge_center.x() + plus_half, badge_center.y()));
    painter->drawLine(QPointF(badge_center.x(), badge_center.y() - plus_half),
                      QPointF(badge_center.x(), badge_center.y() + plus_half));
}

}

GameCard::GameCard(QObject* parent) : QStyledItemDelegate{parent} {
    setObjectName("GameCard");
}

void GameCard::paint(QPainter* painter, const QStyleOptionViewItem& option,
                     const QModelIndex& index) const {
    if (!index.isValid())
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    const QRect cardRect = getCardRect(option, index);
    const bool selected = option.state & QStyle::State_Selected;
    const bool hovered = option.state & QStyle::State_MouseOver;
    const bool add_folder =
        index.data(GameListItem::TypeRole).value<GameListItemType>() == GameListItemType::AddDir;
    const QPalette palette = option.palette;
    const bool dark = palette.window().color().lightness() < 128;
    const int radius = 10;

    QColor surface = dark ? QColor(12, 29, 43) : QColor(255, 255, 255, 245);
    QColor border = dark ? QColor(42, 72, 91) : QColor(24, 34, 54, 18);
    QColor titleColor = palette.text().color();
    QColor subtitleColor = dark ? QColor(145, 166, 183) : palette.text().color();
    subtitleColor.setAlpha(dark ? 230 : 165);

    if (selected) {
        QColor accent = palette.highlight().color();
        surface = accent;
        surface.setAlpha(dark ? 95 : 55);
        border = palette.highlight().color();
        border.setAlpha(190);
        titleColor = palette.highlightedText().color();
        subtitleColor = palette.highlightedText().color();
        subtitleColor.setAlpha(190);
    } else if (hovered) {
        surface = dark ? QColor(15, 37, 52) : QColor(248, 250, 255, 252);
        border = palette.highlight().color();
        border.setAlpha(dark ? 95 : 75);
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(dark ? QColor(0, 0, 0, hovered || selected ? 65 : 40)
                           : QColor(20, 30, 60, hovered || selected ? 28 : 18));
    painter->drawRoundedRect(cardRect.translated(0, selected ? 2 : 3), radius, radius);

    painter->setBrush(surface);
    painter->setPen(QPen(border, 1));
    painter->drawRoundedRect(cardRect, radius, radius);

    const u32 icon_size = UISettings::values.game_icon_size.GetValue();
    const QPixmap icon_pixmap = index.data(GameListItemPath::CardIconRole).value<QPixmap>();
    const bool has_icon = index.data(GameListItemPath::HasIconRole).toBool();
    const bool show_game_name = UISettings::values.show_game_name.GetValue();
    const int inset = cardMargin + 2;
    const int titleHeight = show_game_name ? 74 : 0;
    const QRect artworkRect(cardRect.left() + 1, cardRect.top() + 1,
                            std::max(1, cardRect.width() - 2),
                            std::max(1, cardRect.height() - titleHeight -
                                             (show_game_name ? inset : 2)));
    QPainterPath artwork_clip;
    artwork_clip.addRoundedRect(artworkRect, radius - 1, radius - 1);
    painter->save();
    painter->setClipPath(artwork_clip);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (add_folder) {
        const QColor accent = selected ? palette.highlightedText().color()
                                       : palette.highlight().color();
        DrawAddFolderArtwork(painter, artworkRect, accent, dark);
    } else if (has_icon && !icon_pixmap.isNull()) {
        QSize scaled = icon_pixmap.size();
        scaled.scale(artworkRect.size(), Qt::KeepAspectRatioByExpanding);
        const QRect imageRect(artworkRect.center().x() - scaled.width() / 2,
                              artworkRect.center().y() - scaled.height() / 2, scaled.width(),
                              scaled.height());
        painter->drawPixmap(imageRect, icon_pixmap);
    } else if (!has_icon) {
        DrawSwitch2Placeholder(painter, artworkRect);
    }
    painter->restore();

    if (show_game_name) {
        QRect textRect = cardRect.adjusted(12, 0, -12, -inset);
        textRect.setTop(cardRect.bottom() - inset - titleHeight + 2);

        const QString full = index.data(GameListItemPath::TitleRole).toString();
        const QString title = add_folder
                                  ? tr("Add Games Folder")
                                  : (full.isEmpty()
                                         ? index.data(Qt::DisplayRole)
                                               .toString()
                                               .section(QLatin1Char('\n'), 0, 0)
                                         : full);

        painter->setPen(titleColor);
        QFont titleFont = option.font;
        titleFont.setBold(true);
        titleFont.setPixelSize(std::clamp(
            static_cast<int>(std::sqrt(static_cast<double>(icon_size)) * 1.15), 12, 16));
        painter->setFont(titleFont);
        QRect titleRect = textRect;
        titleRect.setBottom(textRect.top() + titleHeight - 24);
        painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, title);

        QStringList meta_parts;
        if (add_folder) {
            meta_parts.append(tr("Choose your games folder"));
        } else {
            const QString size_text = index.sibling(index.row(), GameListModel::COLUMN_SIZE)
                                          .data(Qt::DisplayRole)
                                          .toString();
            if (!size_text.isEmpty()) {
                meta_parts.append(size_text);
            }
            const QString file_type = index.data(GameListItemPath::FileTypeRole).toString();
            if (!file_type.isEmpty()) {
                meta_parts.append(file_type);
            }
        }
        if (!meta_parts.isEmpty()) {
            const QString meta = meta_parts.join(QStringLiteral(" \u2022 "));
            painter->setPen(subtitleColor);
            QFont metaFont = option.font;
            metaFont.setPixelSize(11);
            painter->setFont(metaFont);
            QRect metaRect = textRect;
            metaRect.setTop(textRect.bottom() - 18);
            painter->drawText(metaRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, meta);
        }
    }

    painter->restore();
}

QRect GameCard::getCardRect(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    const int cell_width = option.rect.width();

    const int card_width = cell_width - m_padding;

    int card_left, card_top, card_height;

    if (m_columns >= 1) {
        // grid mode

        // center everything in-line, such that the leftmost and rightmost cards
        // have ~ equal padding to the edge of the viewport
        // spacing between each card is larger, but equal to each other
        const int column = index.row() % m_columns;

        const int row_width = m_columns * cell_width;
        const int total_gap = row_width - cardMargin * 2 - m_columns * card_width;
        const int gap = (m_columns > 1) ? (total_gap / (m_columns - 1)) : 0;
        card_left =
            option.rect.left() - column * cell_width + cardMargin + column * (card_width + gap) + 4;

        // fill cell vertically
        card_top = option.rect.top() + cardMargin;
        card_height = option.rect.height() - cardMargin - 1;
    } else {
        // carousel mode
        card_left = option.rect.left() + cardMargin + 4;

        // the delegate itself takes up the full height, but the card itself
        // gets centered
        const int content_height = m_contentSize.height() - cardMargin;
        const int cell_height = option.rect.height();

        card_height = std::min(content_height, cell_height - cardMargin * 2) - 1;
        card_top = option.rect.top() + (cell_height - card_height) / 2;
    }

    return QRect(card_left, card_top, card_width - cardMargin, card_height);
}

bool GameCard::hitTest(const QPoint& point, const QModelIndex& index, const QWidget* widget,
                       const QRect& cellRect) const {
    QStyleOptionViewItem option;
    option.initFrom(widget);
    option.rect = cellRect;
    return getCardRect(option, index).contains(point);
}

QSize GameCard::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return m_size;
}

void GameCard::setSize(const QSize& newSize, const QSize& contentSize, const int padding,
                       const int columns) {
    m_size = newSize;
    m_contentSize = contentSize;
    m_padding = padding;
    m_columns = columns;
}

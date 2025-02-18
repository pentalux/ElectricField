#ifndef FIELDWIDGET_H
#define FIELDWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>

struct Charge {
    QPointF position;
    double value;
};

class FieldWidget : public QWidget {
    Q_OBJECT

public:
    explicit FieldWidget(QWidget *parent = nullptr);
    void addCharge(const QPointF &position, double value);
    void clearCharges();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QVector<Charge> charges;
    // Параметры для масштабирования и панорамирования
    double m_scale = 1.0;
    QPointF m_offset = QPointF(0, 0);
    QPoint m_lastPos;

    void drawFieldLines(QPainter &painter);
    QPointF electricFieldAt(const QPointF &point) const;
};

#endif // FIELDWIDGET_H

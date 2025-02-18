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
    double m_scale = 1.0;          // Коэффициент масштабирования
    QPointF m_offset = QPointF(0, 0);  // Смещение (панорамирование)
    QPoint m_lastPos;              // Последняя позиция мыши

    const double minScale = 0.2;    // Минимальный масштаб
    const double maxScale = 5.0;    // Максимальный масштаб
    const double maxPan = 500.0;    // Ограничение панорамирования

    void drawGrid(QPainter &painter);
    void drawFieldLines(QPainter &painter);
    void drawChargesWithLabels(QPainter &painter);
    QPointF electricFieldAt(const QPointF &point) const;
};

#endif // FIELDWIDGET_H

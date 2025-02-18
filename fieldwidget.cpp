#include "fieldwidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FieldWidget::FieldWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(400, 400);
}

void FieldWidget::addCharge(const QPointF &position, double value) {
    charges.append({position, value});
    update();
}

void FieldWidget::clearCharges() {
    charges.clear();
    update();
}

void FieldWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Устанавливаем систему координат: сначала центрируем (0,0) в центре виджета
    painter.translate(width()/2, height()/2);
    // Применяем смещение (панорамирование)
    painter.translate(m_offset);
    // Применяем масштабирование (zoom)
    painter.scale(m_scale, m_scale);


    // Рисуем заряды
    for (const Charge &charge : charges) {
        painter.setBrush(charge.value > 0 ? Qt::red : Qt::blue);
        painter.drawEllipse(charge.position, 5, 5);
    }

    // Рисуем силовые линии поля
    drawFieldLines(painter);
}

void FieldWidget::drawFieldLines(QPainter &painter) {
    // Параметры интегрирования
    const double ds = 5.0;
    const int numLinesPerCharge = 8;
    const double epsilon = 1e-2;
    const int maxSteps = 1000;

    // Для каждого заряда создаём seed-точки, равномерно распределённые вокруг него
    for (const Charge &charge : charges) {
        for (int i = 0; i < numLinesPerCharge; ++i) {
            double angle = 2 * M_PI * i / numLinesPerCharge;
            QPointF direction(std::cos(angle), std::sin(angle));
            QPointF seed = charge.position + direction * 10.0;

            // Интегрирование "вперёд" по направлению поля
            QVector<QPointF> forwardLine;
            QPointF p_forward = seed;
            forwardLine.append(seed);
            for (int j = 0; j < maxSteps; ++j) {
                QPointF E = electricFieldAt(p_forward);
                double E_magnitude = std::hypot(E.x(), E.y());
                if (E_magnitude < epsilon)
                    break;
                QPointF E_unit = E / E_magnitude;
                p_forward += E_unit * ds;
                forwardLine.append(p_forward);
            }

            // Интегрирование "назад" против направления поля
            QVector<QPointF> backwardLine;
            QPointF p_backward = seed;
            backwardLine.append(seed);
            for (int j = 0; j < maxSteps; ++j) {
                QPointF E = electricFieldAt(p_backward);
                double E_magnitude = std::hypot(E.x(), E.y());
                if (E_magnitude < epsilon)
                    break;
                QPointF E_unit = E / E_magnitude;
                p_backward -= E_unit * ds;
                backwardLine.append(p_backward);
            }
            std::reverse(backwardLine.begin(), backwardLine.end());
            QVector<QPointF> fullLine = backwardLine;
            if (!forwardLine.isEmpty() && forwardLine.first() == seed) {
                for (int k = 1; k < forwardLine.size(); ++k)
                    fullLine.append(forwardLine[k]);
            } else {
                fullLine += forwardLine;
            }
            painter.setPen(QPen(Qt::black, 1));
            painter.drawPolyline(fullLine.data(), fullLine.size());
        }
    }
}

QPointF FieldWidget::electricFieldAt(const QPointF &point) const {
    QPointF E(0, 0);
    const double k = 8.9875517873681764e9;
    for (const Charge &charge : charges) {
        QPointF r = point - charge.position;
        double r_sq = r.x()*r.x() + r.y()*r.y();
        double r_mag = std::sqrt(r_sq);
        if (r_mag < 1e-2)
            continue;
        double factor = k * charge.value / (r_sq * r_mag);
        E += r * factor;
    }
    return E;
}

void FieldWidget::wheelEvent(QWheelEvent *event) {// Изменяем масштаб: положительный delta (прокрутка вперед) увеличивает масштаб
    double delta = event->angleDelta().y();
    m_scale *= std::pow(1.001, delta);
    update();
}

void FieldWidget::mousePressEvent(QMouseEvent *event) {
    m_lastPos = event->pos();
}

void FieldWidget::mouseMoveEvent(QMouseEvent *event) {
    QPointF delta = event->pos() - m_lastPos;
    // Применяем смещение с учетом текущего масштаба
    m_offset += delta / m_scale;
    m_lastPos = event->pos();
    update();
}

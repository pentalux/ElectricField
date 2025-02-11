#include "fieldwidget.h"
#include <QPainter>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FieldWidget::FieldWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(400, 400);
}

void FieldWidget::addCharge(const QPointF &position, double value)
{
    charges.append({position, value});
    update();
}

void FieldWidget::clearCharges()
{
    charges.clear();
    update();
}

void FieldWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    // Рисуем заряды: положительный – красный, отрицательный – синий
    for (const Charge &charge : charges) {
        painter.setBrush(charge.value > 0 ? Qt::red : Qt::blue);
        painter.drawEllipse(charge.position, 5, 5);
    }

    // Рисуем силовые линии поля
    drawFieldLines(painter);
}

void FieldWidget::drawFieldLines(QPainter &painter)
{
    // Параметры интегрирования
    const double ds = 5.0;             // шаг интегрирования (в пикселях)
    const int numLinesPerCharge = 8;     // число seed-точек вокруг каждого заряда
    const double epsilon = 1e-2;         // порог прекращения интегрирования
    const int maxSteps = 1000;           // максимум шагов интегрирования

    // Для каждого заряда создаём набор seed-точек, распределённых равномерно по окружности
    for (const Charge &charge : charges) {
        for (int i = 0; i < numLinesPerCharge; ++i) {
            double angle = 2 * M_PI * i / numLinesPerCharge;
            QPointF direction(std::cos(angle), std::sin(angle));
            // Определяем seed-точку на небольшой окружности вокруг заряда (радиус 10 пикселей)
            QPointF seed = charge.position + direction * 10.0;

            // Интегрирование "вперёд" (по направлению поля)
            QVector<QPointF> forwardLine;
            QPointF p_forward = seed;
            forwardLine.append(seed);
            for (int j = 0; j < maxSteps; ++j) {
                QPointF E = electricFieldAt(p_forward);
                double E_magnitude = std::sqrt(E.x()*E.x() + E.y()*E.y());
                if (E_magnitude < epsilon)
                    break;
                QPointF E_unit = E / E_magnitude;
                p_forward += E_unit * ds;
                forwardLine.append(p_forward);
            }

            // Интегрирование "назад" (против направления поля)
            QVector<QPointF> backwardLine;
            QPointF p_backward = seed;
            backwardLine.append(seed);
            for (int j = 0; j < maxSteps; ++j) {
                QPointF E = electricFieldAt(p_backward);
                double E_magnitude = std::sqrt(E.x()*E.x() + E.y()*E.y());
                if (E_magnitude < epsilon)
                    break;
                QPointF E_unit = E / E_magnitude;
                p_backward -= E_unit * ds;  // идём в обратную сторону
                backwardLine.append(p_backward);
            }
            // Разворачиваем backwardLine, чтобы соединить с forwardLine
            std::reverse(backwardLine.begin(), backwardLine.end());

            // Объединяем backwardLine и forwardLine (убираем дублирование seed)
            QVector<QPointF> fullLine = backwardLine;
            // Если первый элемент forwardLine совпадает с seed, пропускаем его
            if (!forwardLine.isEmpty() && forwardLine.first() == seed) {
                for (int k = 1; k < forwardLine.size(); ++k)
                    fullLine.append(forwardLine[k]);
            } else {
                fullLine += forwardLine;
            }

            // Рисуем полученную линию
            painter.setPen(QPen(Qt::black, 1));
            painter.drawPolyline(fullLine.data(), fullLine.size());
        }
    }
}

QPointF FieldWidget::electricFieldAt(const QPointF &point) const
{
    // Вычисляем суммарное электрическое поле в точке point
    QPointF E(0, 0);
    // Используем константу Кулона (можно подбирать под масштабы)
    const double k = 8.9875517873681764e9;
    for (const Charge &charge : charges) {
        QPointF r = point - charge.position;
        double r_sq = r.x()*r.x() + r.y()*r.y();
        double r_mag = std::sqrt(r_sq);
        // Избегаем деления на ноль (если слишком близко к заряду, пропускаем вклад)
        if (r_mag < 1e-2)
            continue;
        double factor = k * charge.value / (r_sq * r_mag);
        E += r * factor;
    }
    return E;
}

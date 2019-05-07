#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <tuple>
#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QIODevice>
#include <QtCharts>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer *timer;
    QLineSeries *series;

private slots:
    void check_cpu();

private:
    Ui::MainWindow *ui;
    unsigned int user, total, iter = 0;
    QVector<double> list;
    QChartView *chartView;
    QChart *chart;
    std::tuple<unsigned int, unsigned int> readFile();
};

#endif // MAINWINDOW_H

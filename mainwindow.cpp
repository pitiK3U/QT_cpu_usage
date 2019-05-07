#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    try {
        // Gets the first number of processes
        std::tuple<unsigned int, unsigned int> tuple = readFile();
        user = std::get<0>(tuple);
        total = std::get<1>(tuple);
    } catch (QFileDevice::FileError) {
        // Second catch will close the window because this is before
        // window is even created.
    }


    // Make function for checking CPU every 1 sec
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(check_cpu()));
    timer->start(1000);

    // Creates new QChart dynamically
    series = new QLineSeries();
    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("CPU usage in %");

    //Creates View for QChart
    chartView = new QChartView(chart);

    // Adds QChartView to the gridLayout so it scales dynamically
    ui->gridLayout->addWidget(chartView, 1, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Calculates the usage of cpu for user and whole computer
// and updates the QGraph showed on the main window
void MainWindow::check_cpu() {
    try {
        // Declares used variables for calculation
        unsigned int total_now = 0, user_now = 0;
        std::tuple<unsigned int, unsigned int> tuple = readFile();
        user_now = std::get<0>(tuple);
        total_now = std::get<1>(tuple);

        // The usage is ratio of user's usage and total usage
        // within a certain period of time.
        unsigned int user_over_period = user_now - user, total_over_period = total_now - total;
        double cpu_usage = static_cast<double>(user_over_period)/total_over_period * 100;

        // Adds new point to graph series.
        // Keeps the maximum of points to 10 and updates
        // the chart.
        series->append(iter, cpu_usage);
        iter++;
        if (series->count() > 10) series->remove(0);
        chart->removeSeries(series);
        chart->addSeries(series);
        chart->createDefaultAxes();

        ui->label->setText("CPU: " + QString::number(cpu_usage) + "%");

        // Sets the current number of processes
        // to a global value for next iteration.
        total = total_now;
        user = user_now;

    } catch (QFileDevice::FileError) {
        // If file wasn't found, user will be prompt with dialog window
        // and program will close itself.
        QMessageBox::critical(this, "Can't read file /proc/stat", "Please install Linux, ty! :D");
        this->close();
    }

}

// Returns tuple of current number of processes of user and computer
// for current cycle. Reads required data from Linux file /proc/stat
// and converts it to a unsigned integer.
std::tuple<unsigned int, unsigned int> MainWindow::readFile() {
    char line[40];
    unsigned int total_now = 0, user_now = 0, pos = 0;

    QString filename = "/proc/stat";
    QFile file(filename);

    //Tries to open the file else throws error.
    if(file.open(QIODevice::ReadOnly)) {
        file.readLine(line, 40);
        char* str = strtok(line," ");
        while (str != nullptr) {
            total_now += std::strtoul(str, nullptr, 0);
            if(pos < 4) user_now += std::strtoul(str, nullptr, 0);
            str = strtok(nullptr, " ");
            pos++;
        }
        return std::make_tuple(user_now, total_now);
    } else {
        throw file.error();
    }

}

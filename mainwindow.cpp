#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Transform.h"
#include "helpers.h"
#include <iostream>
#include "test.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Вхідні дані
    std::array<double, 3> input = {4.0f, 16.0f, 25.0f};

    // Створення трансформатора
    Transform<3, float, true, Multiply, Add, Break, Multiply> transform {
        Multiply(2.0f),    // Помножити на 2
        Add(0.0f),         // Додати 3
        Break(),           // Точка розділу
        Multiply(2.0f)
    };

    qDebug() << "DataSize: " << transform.DataSize;
    qDebug() << "TransformSize: " << transform.TransformSize;
    qDebug() << "BreakIndex: " << transform.BreakIndex;
    qDebug() << "BreakExists: " << transform.BreakExists;
    qDebug() << "BeforeBreakCount: " << transform.BeforeBreakCount;
    qDebug() << "AfterBreakCount: " << transform.AfterBreakCount;

    // // Виконання першої частини
    transform.process(input);

    // Отримання проміжних результатів
    auto intermediate = transform.get_array();
    std::cout << "(before Break):\n";
    for (auto value : intermediate) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    // Виконання другої частини (results)
    auto finalResults = transform.results();
    std::cout << "Фінальні результати (після Break):\n";
    for (auto value : finalResults) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    test();

}

MainWindow::~MainWindow()
{
    delete ui;
}

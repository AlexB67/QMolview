#include "spectrum_window.h"

void SpectrumWindow::create_freq_table()
{
    freq_table = new QTableWidget(this);
    freq_table->setSortingEnabled(false);
    freq_table->insertColumn(0);
    freq_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    freq_table->setColumnCount(3);
    QStringList header;
    header << tr("𝜈 / cm\u207B\u00B9") <<  tr("𝐼 / km mol\u207B\u00B9") << tr("𝜀 ₘ / L mol\u207B\u00B9 cm\u207B\u00B9");
    freq_table->setHorizontalHeaderLabels(header);
  //  freq_table->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    freq_table->setColumnWidth(0, 140);
    freq_table->setColumnWidth(1, 140);
    freq_table->setColumnWidth(2, 150);
    freq_table->setMinimumWidth(440);

    freq_table->setSelectionMode(QAbstractItemView::SingleSelection);
    freq_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    freq_table->setToolTip(tr("Select a row to higlight a peak."));
}

void SpectrumWindow::init_freq_table()
{
    QStringList headers;
    table_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

#ifdef Q_OS_WIN
    table_font.setFamily(tr("Lucida Console"));
    table_font.setPointSizeF(1.2 * table_font.pointSizeF());
#else
    table_font.setStyleHint(QFont::TypeWriter);
#endif

    const auto& ir_freq = frequencies;

    for (int i = 0; i < freq_Items.size(); ++i)
    {
        delete freq_Items[i];
        delete intensity_Items[i];
        delete absorp_Items[i];
    }

    freq_Items.clear();
    intensity_Items.clear();
    absorp_Items.clear();
    freq_table->setRowCount(0);

    const double fwhh = FWHH_spinner->value();
    const double scaling = freq_scaling->value() / 100.0;
    // 27.648045421; Naperian convention conversion factor
    const double prefac = 2.0 * 100.0 / (fwhh * M_PI * std::log(10.0));

    int k = 0;
    for (const auto &f : ir_freq)
    {
        freq_Items.push_back(new QTableWidgetItem);
        intensity_Items.push_back(new QTableWidgetItem);
        absorp_Items.push_back(new QTableWidgetItem);

        freq_table->insertRow(k);
        if (k < 9)
            headers << QString("   ") + QString::number(k + 1);
        else
            headers << QString("  ") + QString::number(k + 1);

        freq_Items[k]->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        freq_Items[k]->setText(QString::number(f * scaling, 'f', 4) + QString("    "));
        freq_Items[k]->setFont(table_font);
        freq_table->setItem(k, 0, freq_Items[k]);

        intensity_Items[k]->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        intensity_Items[k]->setText(QString::number(intensities[k], 'f', 4) + QString("    "));
        intensity_Items[k]->setFont(table_font);
        freq_table->setItem(k, 1, intensity_Items[k]);

        absorp_Items[k]->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        absorp_Items[k]->setText(QString::number(intensities[k] * prefac, 'f', 4) + QString("    "));
        absorp_Items[k]->setFont(table_font);
        freq_table->setItem(k, 2, absorp_Items[k]);
        ++k;
    }

    freq_table->setVerticalHeaderLabels(headers);
}

void SpectrumWindow::update_freq_table()
{
    const double fwhh = FWHH_spinner->value();
    const double prefac = 2.0 * 100.0 / (fwhh * M_PI * std::log(10.0));

    for (int k = 0; k < freq_Items.size(); ++k)
    {
        freq_Items[k]->setText(QString::number(frequencies[k] * scale_vec[k], 'f', 4) + QString("    "));
        intensity_Items[k]->setText(QString::number(intensities[k], 'f', 4) + QString("    "));
        absorp_Items[k]->setText(QString::number(intensities[k] * prefac, 'f', 4) + QString("    "));
    }
}

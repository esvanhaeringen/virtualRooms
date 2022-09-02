#include "monitorWidget.h"
#include <QHeaderView>

MonitorWidget::MonitorWidget(QArrow2D* parent)
    : d_parent(parent)
{
    this->setRowCount(d_nItems);
    this->setColumnCount(2);
    QStringList colHeaders;
    colHeaders << "Property" << "Value";
    this->setHorizontalHeaderLabels(colHeaders);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setFocusPolicy(Qt::NoFocus);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->verticalHeader()->hide();
}

void MonitorWidget::setup()
{
    if (d_parent->model() != nullptr)
    {
        d_nItems = 16;
        this->setRowCount(d_nItems);
        for (int i = 0; i < d_nItems; i++)
        {
            this->setItem(i, 0, new QTableWidgetItem);
            this->setItem(i, 1, new QTableWidgetItem);
        }
        this->item(0, 0)->setText("ID");
        this->item(1, 0)->setText("Group");
        this->item(2, 0)->setText("Room number");
        this->item(3, 0)->setText("Valence");
        this->item(4, 0)->setText("Arousal");
        this->item(5, 0)->setText("Expressed Valence");
        this->item(6, 0)->setText("Expressed Arousal");
        this->item(7, 0)->setText("Emotion label");
        this->item(8, 0)->setText("Expressivity");
        this->item(9, 0)->setText("Susceptibility");
        this->item(10, 0)->setText("Decay speed");
        this->item(11, 0)->setText("Pos. stimulus bias");
        this->item(12, 0)->setText("Neg. stimulus bias");
        this->item(13, 0)->setText("Attention bias");
        this->item(14, 0)->setText("Att. pref. Valence");
        this->item(15, 0)->setText("Att. pref. Arousal");
    }
}

void MonitorWidget::update()
{
    const char* labels[] = { "Neutral",
            "Pleased/relaxed",
            "Sleepy/peaceful",
            "Tired/bored",
            "Miserable/sad",
            "Distressed/frustrated",
            "Angry/afraid",
            "Aroused/excited",
            "Happy/delighted" };
    if (this != nullptr)
    {
        if (d_agent != -1)
        {
            if (this->rowCount() == 0)
                setup();
            Agent const& agent = d_parent->model()->agent(d_agent);
            this->item(0, 1)->setText(QString::fromStdString(agent.id()));
            this->item(1, 1)->setText(QString::fromStdString(d_parent->model()->groupId(agent.group())));
            this->item(2, 1)->setText(QString::fromStdString(d_parent->model()->roomId(agent.room())));
            this->item(3, 1)->setText(QString::number(agent.emotionMdl().emotion().valence()));
            this->item(4, 1)->setText(QString::number(agent.emotionMdl().emotion().arousal()));
            this->item(5, 1)->setText(QString::number(agent.emotionMdl().expression().valence()));
            this->item(6, 1)->setText(QString::number(agent.emotionMdl().expression().arousal()));
            this->item(7, 1)->setText(QString::fromStdString(agent.emotionMdl().emotionLabel()));
            this->item(8, 1)->setText(QString::number(agent.emotionMdl().expressivity()));
            this->item(9, 1)->setText(QString::number(agent.emotionMdl().susceptibility()));
            this->item(10, 1)->setText(QString::number(agent.emotionMdl().regulationEfficiency()));
            this->item(11, 1)->setText(QString::number(agent.emotionMdl().posStimulusBias()));
            this->item(12, 1)->setText(QString::number(agent.emotionMdl().negStimulusBias()));
            this->item(13, 1)->setText(QString::number(agent.emotionMdl().attentionBias()));
            this->item(14, 1)->setText(QString::number(agent.emotionMdl().attPrefVal()));
            this->item(15, 1)->setText(QString::number(agent.emotionMdl().attPrefAro()));
        }
    }
}

void MonitorWidget::clean()
{
    d_agent = -1;
    this->setRowCount(0);
}

//[ ACCESSORS ]
int const MonitorWidget::agent() const
{
    return d_agent;
}

//[ MODIFIERS ]
void MonitorWidget::setAgent(int agent)
{
    d_agent = agent;
}


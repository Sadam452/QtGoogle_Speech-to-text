#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->setReadOnly(false);
    //recording
    m_audioRecorder = new QAudioRecorder();
    m_probe = new QAudioProbe();

    //channels
    ui->comboBox->addItem(tr("English"), QVariant("en-US"));
    ui->comboBox->addItem(QStringLiteral("English(India)"), QVariant("en-IN"));
    ui->comboBox->addItem(QStringLiteral("Bengali"), QVariant("bn-IN"));
    ui->comboBox->addItem(QStringLiteral("Gujarati"), QVariant("gu-IN"));
    ui->comboBox->addItem(QStringLiteral("Hindi"), QVariant("hi-IN"));
    ui->comboBox->addItem(QStringLiteral("Kannada"), QVariant("ka-IN"));
    ui->comboBox->addItem(QStringLiteral("Malayalam"), QVariant("ml-IN"));
    ui->comboBox->addItem(QStringLiteral("Marathi"), QVariant("mr-IN"));
    ui->comboBox->addItem(QStringLiteral("Tamil"), QVariant("ta-IN"));
    ui->comboBox->addItem(QStringLiteral("Telugu"), QVariant("te-IN"));
    ui->comboBox->addItem(QStringLiteral("Urdu"), QVariant("ur-IN"));
    ui->comboBox->addItem(QStringLiteral("Punjabi"), QVariant("pa-Guru-IN"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionSave_triggered()
{
    QString file(QFileDialog::getSaveFileName(this, "Open a File"));
    QFile File(file);
    if(!File.open(QFile::WriteOnly | QFile::Text))return;
    QTextStream out(&File);
    out.setCodec("UTF-8");
    QString text = ui->textBrowser->toPlainText();
    out << text;
    File.flush();
    File.close();
}

/*!
 * \brief MainWindow::speechToTextCall
 * \details This function is called when recording is stopped.
 * \details This function sends request to Google Speech-to-text api and receives back response text.
 * \details Response text is pasted at cursor position.
 */
void MainWindow::speechToTextCall()
{
    QString fileName = QDir::currentPath() + "/audio.wav";
    QFile audioFile(fileName);
    if(!audioFile.open(QIODevice::ReadOnly)){
        QMessageBox::critical(0,"Error","Error recording your audio! Try again");
        ui->pushButton_4->setText("Speech to text");
        return;
    }

    int idx = ui->comboBox->currentIndex();
    QString enc;
    if (idx == -1)
        enc = "en-US";
    else
        enc = ui->comboBox->itemData(idx).toString();
    QByteArray audioData=audioFile.readAll();

    QUrl url("https://speech.googleapis.com/v1/speech:recognize");
    QUrlQuery query;
    QString myKey = "speech-to-text api key";
    query.addQueryItem("key",myKey);
    url.setQuery(query);
    QNetworkRequest request(url);
#ifdef Q_OS_WIN
    request.setHeader(QNetworkRequest::ContentTypeHeader,"audio/pcm");
#else
    request.setHeader(QNetworkRequest::ContentTypeHeader,"audio/x-flac");
#endif

    QJsonObject json;
    QJsonObject config;
#ifdef Q_OS_WIN
    config["encoding"]="LINEAR16";
#else
    config["encoding"]="FLAC";
#endif
    config["sampleRateHertz"]=44100;
    config["languageCode"]=enc;
    json["config"]=config;
    QJsonObject audio;
    audio["content"]=QString::fromLatin1(audioData.toBase64());
    json["audio"]=audio;
    QByteArray jsonData=QJsonDocument(json).toJson();

    QNetworkAccessManager *manager=new QNetworkAccessManager();
    QNetworkReply *reply=manager->post(request,jsonData);

    QObject::connect(reply,&QNetworkReply::finished,[this,reply](){
        if(reply->error()!=QNetworkReply::NoError){
            QMessageBox::critical(0,"Error Occured","Error connecting to server...Please try again after some time");
            ui->pushButton_4->setText("Speech to text");
            return;
        }
        else if(reply->error()==QNetworkReply::UnknownNetworkError){
            QMessageBox::warning(0,"Network Error","Please check your internet connection and try again!");

        }
        else if(reply->isFinished() && reply->error()==QNetworkReply::NoError){

            QJsonDocument responseJson=QJsonDocument::fromJson(reply->readAll());
            QJsonObject object=responseJson.object();
            QString ResponseText=object["results"].toArray()[0].toObject()
                    ["alternatives"].toArray()[0].toObject()["transcript"].toString();
            QTextCursor cur = ui->textBrowser->textCursor();
            cur.insertText(ResponseText);
        }
        ui->pushButton_4->setText("Speech to text");

        reply->deleteLater();
    });
}

/*!
 * \brief MainWindow::on_pushButton_4_clicked
 * \details User needs to select a language from language drop down menu(Language in which user will record an audio).
 * \details When "Speech to text" button is clicked, this functon is called. This function records the user audio and shows status on the same button.
 * \details "Speech to text" status means user can start recording. "Stop ?" means audio recording is in progress and user can stop it by clicking the "Stop ?" button.
 * \details "Processing ..." means the user input is in process and the requested audio will be converted into text and pasted at cursor position. Text is inserted at cursor position
 * \details and status of button is changed back to "Speech to text".
 */
void MainWindow::on_pushButton_4_clicked()
{
    if (m_audioRecorder->state() == QMediaRecorder::StoppedState) {
        QString fileName = QDir::currentPath() + "/audio.wav";
        m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
        qDebug()<<"Recording your audio!!";
        ui->pushButton_4->setText("Stop ?");
        QAudioEncoderSettings settings;
#ifdef Q_OS_WIN
        settings.setCodec("audio/pcm");
#else
        settings.setCodec("audio/x-flac");
#endif
        settings.setSampleRate(0);
        settings.setBitRate(0);
        settings.setChannelCount(1);
        settings.setQuality(QMultimedia::EncodingQuality(2));
        settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);

        m_audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), "");
        m_audioRecorder->record();
    }
    else {
        qDebug()<<"stopped your recording!";
        ui->pushButton_4->setText("Processing ...");
        m_audioRecorder->stop();
        speechToTextCall();
    }
}

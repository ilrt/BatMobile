/*
 * Copyright (c) 2012, Chris Scott, C.D.Scott@leeds.ac.uk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DRAW_H
#define DRAW_H

#include "sndIO.h"
#include "fft.h"
#include "filter.h"
#include "utilities.h"
#include <deque>
#include <QImage>
#include <QPainter>
#include <QFileDialog>

class Draw {

public:
    Draw(QString _dir);
    ~Draw();
    void process(const std::vector<float> &audioSamples, const Event &event, const double &cutoff, window_type win);
    void process(const Event &event, const double &cutoff, window_type win);
    deque<string> getFiles();
    void pop_back();

private:
    void drawSpectrum(const vector<double> &spectrum, const int &x);
    void drawLine(const int &x);
    void drawPoint(const int &x, const int &y);
    void drawPoint2(const int &x, const int &y);
    void drawText(const int &i, const int &n, const std::string &classification);

    QString dir;
    SndFile snd;
    FFT fft;
    QPainter paint;
    QPainterPath path;
    QPainterPath path2;
    int height;
    int width;
    int scaleX, scaleY, edge;
    deque<std::string> files;
};

inline Draw::Draw(QString _dir)
{
    dir = _dir;
    scaleX = 2;
    scaleY = 4;
    edge = 16;
}

inline Draw::~Draw()
{
    //
}

void Draw::process(const std::vector<float> &audioSamples, const Event &event, const double &cutoff, window_type win)
{
    int fftSize = event.fftSize;
    int stepSize = event.stepSize;
    fft.setPlan(fftSize, win, FFTW_ESTIMATE);
    scaleY = 4;
    scaleX = 1;

    int samplerate = snd.getSamplerate();
    if(samplerate < 192000) {
        samplerate *= 10;
    }

    int seekPoint = event.startSample;
    width = event.freqTrack.size();
    width += 2*edge;
    seekPoint -= edge*stepSize;
    height = fftSize*0.5;

    QImage myPNG(width, height, QImage::Format_ARGB32);
    myPNG.fill(qRgba(255, 255, 255, 255));//
    paint.begin(&myPNG);
    path.moveTo(scaleX*edge, scaleY*event.freqTrack[0]);// initial point
    path2.moveTo(scaleX*edge, scaleY*event.kalmanTrack[0]);// initial point

    int index = 0;
    for(int i=0; i<width; ++i) {
        // Send audio samples to FFT
        fft.process(audioSamples, seekPoint);
        seekPoint += stepSize;// advance STFT

        // FFT spectrum
        vector<double> spectrum = fft.getSpectrum();
        //spectrum = filter.highPass(spectrum, cutoff);
        smoothSpectrum(spectrum, event.smoothing);
        drawSpectrum(spectrum, i);
        if(i>=edge && i<width-edge) drawPoint2(scaleX*i, scaleY*event.kalmanTrack[index]);
        if(i>=edge && i<width-edge) drawPoint(scaleX*i, scaleY*event.freqTrack[index++]);
    }

    paint.end();
    myPNG = myPNG.scaled(scaleX*width, scaleY*height);
    paint.begin(&myPNG);

    //paint.setRenderHint(QPainter::Antialiasing);
    QPen pen(QColor(0, 0, 255, 120));
    pen.setWidth(2);
    paint.setPen(pen);
    paint.drawPath(path);
    path = QPainterPath();

    QPen pen2(QColor(255, 0, 0, 120));
    pen2.setWidth(2);
    paint.setPen(pen2);
    paint.drawPath(path2);
    path2 = QPainterPath();

    // Save spectrogram
    QVariant num(event.id);
    QString append = "_";
    append.append(num.toString());
    append.append(".png");

    QString str = QString::fromStdString(event.filepath);
    QFileInfo fileInfo(str);
    QString filename(fileInfo.fileName());

    str = filename.section(".", -1, -1);// get ext
    QString ext = ".";
    ext.append(str);// ".ext"
    filename.replace(ext, append, Qt::CaseInsensitive);

    str = dir;
    QString folder = str.append("\\spectrograms");
    if(!QDir(folder).exists()) {
        QDir().mkdir(folder);
    }

    QString pathname = str.append("\\");
    pathname.append(filename);
    //printf("%s\n", pathname.toStdString().c_str());

    QImage flip = myPNG.mirrored();
    paint.end();
    paint.begin(&flip);
    drawText(0, event.id, event.classification);
    flip.save(pathname);
    paint.end();
    files.push_back(pathname.toStdString());// store filename of spectrogram
}

void Draw::process(const Event &event, const double &cutoff, window_type win)
{
    int fftSize = event.fftSize;
    int stepSize = event.stepSize;
    fft.setPlan(fftSize, win, FFTW_ESTIMATE);
    if(fftSize==256) {
        scaleY = 4;
        scaleX = 1;
    }
    else if(fftSize==512) {
        scaleY = 2;
    }
    else {
        scaleY = 1;
    }
    //Filter filter;
    //filter.initialise(event.sampleRate, fftSize);

    snd.read(event.filepath, true);
    int samplerate = snd.getSamplerate();
    if(samplerate < 192000) {
        samplerate *= 10;
    }
    if(samplerate < 500000) snd.resample(samplerate);
    vector<float> audioSamples = snd.getAudioSamples();
    int seekPoint = event.startSample;

    width = event.freqTrack.size();
    width += 2*edge;
    seekPoint -= edge*stepSize;
    height = fftSize*0.5;

    QImage myPNG(width, height, QImage::Format_ARGB32);
    myPNG.fill(qRgba(255, 255, 255, 255));//
    paint.begin(&myPNG);
    path.moveTo(scaleX*edge, scaleY*event.freqTrack[0]);// initial point
    path2.moveTo(scaleX*edge, scaleY*event.kalmanTrack[0]);// initial point

    int index = 0;
    for(int i=0; i<width; ++i) {
        // Send audio samples to FFT
        fft.process(audioSamples, seekPoint);
        seekPoint += stepSize;// advance STFT

        // FFT spectrum
        vector<double> spectrum = fft.getSpectrum();
        //spectrum = filter.highPass(spectrum, cutoff);
        smoothSpectrum(spectrum, event.smoothing);
        drawSpectrum(spectrum, i);
        if(i>=edge && i<width-edge) drawPoint2(scaleX*i, scaleY*event.kalmanTrack[index]);
        if(i>=edge && i<width-edge) drawPoint(scaleX*i, scaleY*event.freqTrack[index++]);
    }

    paint.end();
    myPNG = myPNG.scaled(scaleX*width, scaleY*height);
    paint.begin(&myPNG);

    //paint.setRenderHint(QPainter::Antialiasing);
    QPen pen(QColor(0, 0, 255, 120));
    pen.setWidth(2);
    paint.setPen(pen);
    paint.drawPath(path);
    path = QPainterPath();

    QPen pen2(QColor(255, 0, 0, 120));
    pen2.setWidth(2);
    paint.setPen(pen2);
    paint.drawPath(path2);
    path2 = QPainterPath();

    // Save spectrogram
    QVariant num(event.id);
    QString append = "_";
    append.append(num.toString());
    append.append(".png");

    QString str = QString::fromStdString(event.filepath);
    QFileInfo fileInfo(str);
    QString filename(fileInfo.fileName());

    str = filename.section(".", -1, -1);// get ext
    QString ext = ".";
    ext.append(str);// ".ext"
    filename.replace(ext, append, Qt::CaseInsensitive);

    str = dir;
    QString folder = str.append("\\spectrograms");
    if(!QDir(folder).exists()) {
        QDir().mkdir(folder);
    }

    QString pathname = str.append("\\");
    pathname.append(filename);
    //printf("%s\n", pathname.toStdString().c_str());

    QImage flip = myPNG.mirrored();
    paint.end();
    paint.begin(&flip);
    drawText(0, event.id, event.classification);
    flip.save(pathname);
    paint.end();
    files.push_back(pathname.toStdString());// store filename of spectrogram
}

inline deque<string> Draw::getFiles() {
    return files;
}

inline void Draw::pop_back() {
    if(!files.empty()) files.pop_back();
}

void Draw::drawSpectrum(const vector<double> &spectrum, const int &x)
{
    double r,g,b;
    for(unsigned int j=0; j<spectrum.size(); ++j) {
        double tmp = spectrum[j];
        tmp = to_dB(tmp);
        tmp = mapRange(tmp, -96.0, 0.0, 0.0, 1.0); // convert dB to scale 0-1
        r = g = b = (1.0-tmp);
        QRgb px = qRgba(static_cast<int>(r*255), static_cast<int>(g*255), static_cast<int>(b*255), 255);
        paint.setPen(px);
        paint.drawPoint(x, j);
    }
}

inline void Draw::drawLine(const int &x)
{
    QRgb px = qRgb(0,0,255);
    paint.setPen(px);
    paint.drawLine(x, 0, x, height);
}

inline void Draw::drawPoint(const int &x, const int &y)
{
    // draw harmonic as a fluid path
    path.lineTo(x, y);
}

inline void Draw::drawPoint2(const int &x, const int &y)
{
    // draw harmonic as a fluid path
    path2.lineTo(x, y);
}

inline void Draw::drawText(const int &i, const int &n, const std::string &classification)
{
    QPen pen(qRgba(0, 0, 255, 255));
    paint.setPen(pen);
    QFont font("Courier", 14, QFont::Bold, true);
    paint.setFont(font);
    paint.drawText(i+2, scaleY*height-10, QString::number(n));  // Draw a number on the image
    if(classification.compare("unknown")) {
        paint.drawText(i+2, scaleY*height-26, QString::fromStdString(classification));
    }
}

#endif // DRAW_H

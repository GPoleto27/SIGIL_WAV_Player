#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <math.h>
#include "sl.h"

int main ()
{
    SetConsoleTitle("SIGIL WAV Player");
    HWND wnd = FindWindow(0, "SIGIL WAV Player");
    while (wnd == NULL)
    {
        wnd = FindWindow(0, "SIGIL WAV Player");
    }
    ShowWindow(wnd, SW_HIDE);
    slWindow(300, 100, "SIGIL WAV Player", 0);
    slSetFont(slLoadFont("arial_narrow_7.ttf"), 20);
    slSetTextAlign(SL_ALIGN_CENTER);
    slSetBackColor(0.0, 0.0, 0.0);

    FILE *fileWAV;
    long long unsigned int fileSize = 0;
    unsigned int pArquivoWAV [20];
    register int i = 0, a = 0;
    short int soundTrigger = 0, pauseTrigger = 0, fileTrigger = 0, loopTrigger = 0;
    int qtdSlash = 0, size = 0, Bits = 0, ByteRate = 0, loadWAV = 0, tex = slLoadTexture("SWAV.png");
    char filename[256]= {0}, *newLpstrFile = 0, v[4][10], vFinal[9], bits[2], bFinal[3];
    double fileLength = 0, fileElapsed = 0;
    long long unsigned int currentTime = 0, zeroTime = 0, zeroPausedTime = 0, pausedTime = 0, lastPausedTime = 0, timeElapsed = 0;
    OPENFILENAME    ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Áudio WAV (*.wav)\0*.wav\0\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = 256;
    ofn.lpstrTitle = "Selecione o arquivo";
    ofn.Flags = OFN_FILEMUSTEXIST;

    while(!slShouldClose() && !slGetKey(SL_KEY_ESCAPE))
    {
        slSetForeColor(1.0, 1.0, 1.0, 1.0);
        slSprite(tex, 275, 25, 40, 40);
        slText(45, 75, "ARQUIVO");
        slText(270, 75, "LOOP");
        slRectangleOutline(150, 60, 280, 10);   ///Barra
        slRectangleFill(75, 25, 30, 30);        ///Stop
        slTriangleFill(150, 25, 35, 35);        ///Play
        slRectangleFill(215, 25, 10, 30);       ///Pause
        slRectangleFill(230, 25, 10, 30);

        currentTime = GetTickCount();
        if (fileTrigger)
        {
            slSetForeColor(0.0, 0.8, 0.2, 1.0);
            fileElapsed = (timeElapsed*100)/fileLength;
            slSetForeColor(0.0, 0.8, 0.2, 1.0);
            if (fileElapsed <= 100)
                slRectangleFill(10+fileElapsed*1.4,60,fileElapsed*2.8,9);
            slSetForeColor(1.0, 1.0, 1.0, 1.0);
            if (timeElapsed >= fileLength)
            {
                timeElapsed = 0;
                pausedTime = 0;
                lastPausedTime = 0;
                zeroTime = 0;
                zeroPausedTime = 0;
                slSoundStopAll();
                soundTrigger = 0;
                if (loopTrigger)
                {
                    slSoundPlay(loadWAV);
                    zeroTime = GetTickCount();
                    soundTrigger = 1;
                }
            }
            if (pauseTrigger)
            {
                pausedTime = currentTime - zeroPausedTime;
                slSetForeColor(1.0, 1.0, 0.0, 1.0);
                slText(150, 75, "PAUSADO");
                slRectangleFill(10+fileElapsed*1.4,60,fileElapsed*2.8,9);
            }
            else if (soundTrigger)
            {
                timeElapsed = currentTime - zeroTime - pausedTime - lastPausedTime;
                slSetForeColor(0.0, 0.8, 0.2, 1.0);
                slText(150, 75, "TOCANDO");
            }
            else
            {
                slSetForeColor(1.0, 0.1, 0.1, 1.0);
                slText(150, 75, "PARADO");
            }
            if (slGetKey(SL_KEY_ENTER))
            {
                if (!soundTrigger)
                {
                    slSoundPlay(loadWAV);
                    zeroTime = GetTickCount();
                    soundTrigger  = 1;
                    Sleep(100);
                }
                else if (pauseTrigger)
                {
                    slSoundResumeAll();
                    pauseTrigger = 0;
                    Sleep(100);
                }
                else if (soundTrigger)
                {
                    zeroPausedTime = GetTickCount();
                    lastPausedTime += pausedTime;
                    slSoundPauseAll();
                    pauseTrigger = 1;
                    Sleep(100);
                }
            }
        }
        else
        {
            slSetForeColor(1.0, 0.1, 0.1, 1.0);
            slText(150, 75, "PARADO");
        }

        ///Arquivo
        if (slGetMouseX() >= 10 && slGetMouseX() <= 75 && slGetMouseY() >= 75 && slGetMouseY() <= 90)
        {
            slSetForeColor(0.0, 0.8, 0.2, 1.0);
            slText(45, 75, "ARQUIVO");
            if(slGetMouseButton(SL_MOUSE_BUTTON_LEFT))
            {
                if (GetOpenFileName(&ofn))
                {
                    size = strlen(ofn.lpstrFile);
                    for(i = 0; i < size; i++)
                        if(ofn.lpstrFile[i] == '\\')
                            qtdSlash++;
                    newLpstrFile = malloc(sizeof *newLpstrFile * (size+qtdSlash + 1));
                    for(i = 0, a = 0; ofn.lpstrFile[i]; i++, a++)
                        if(ofn.lpstrFile[i] == 92)
                        {
                            newLpstrFile[a++] = 92;
                            newLpstrFile[a] = 92;
                        }
                        else
                            newLpstrFile[a] = ofn.lpstrFile[i];
                    newLpstrFile[a] = '\0';

                    fileWAV = fopen(newLpstrFile, "r");
                    fseek(fileWAV, 34, SEEK_SET);
                    i = 2;
                    while (i--)
                        bits[i] = fgetc(fileWAV);
                    for (i = 0; i < 2; i++)
                        sprintf(v[i], "%x", bits[i]);
                    strcpy(bFinal, "");
                    for (i = 0; i < 2; i++)
                        strcat(bFinal, v[i]);
                    Bits = (int)strtol(bFinal, NULL, 16);

                    if (Bits != 8 && Bits != 16)
                        MessageBox (NULL, "O arquivo deve ser 8 ou 16 bits.", "Falha", MB_OK);
                    else
                    {
                        loadWAV = slLoadWAV(newLpstrFile);
                        free(newLpstrFile);
                        fseek(fileWAV, 28, SEEK_SET);
                        i = 4;
                        while (i--)
                            pArquivoWAV[i] = fgetc(fileWAV);
                        for (i = 0; i < 4; i++)
                            sprintf(v[i], "%x", pArquivoWAV[i]);
                        strcpy(vFinal, "");
                        for (i = 0; i< 4; i++)
                            strcat(vFinal, v[i]);
                        ByteRate = (int)strtol(vFinal, NULL, 16);
                        fseek(fileWAV, 0, SEEK_END);
                        fileSize = ftell(fileWAV);
                        rewind(fileWAV);
                        fclose(fileWAV);
                        fileLength = ((double)fileSize/ByteRate)*1000;

                        slSoundStopAll();
                        currentTime = 0;
                        zeroTime = 0;
                        zeroPausedTime = 0;
                        pausedTime = 0;
                        lastPausedTime = 0;
                        timeElapsed = 0;
                        soundTrigger = 0;
                        fileTrigger = 1;
                        MessageBox(NULL, ofn.lpstrFile, "Arquivo Aberto", MB_OK);
                    }
                }
                else
                    MessageBox (NULL, "O arquivo não pôde ser aberto.", "Falha", MB_OK);
            }
        }

        ///Loop
        if (loopTrigger)
        {
            slSetForeColor(0.0, 0.8, 0.2, 1.0);
            slText(270, 75, "LOOP");
        }
        if(slGetMouseX() >= 250  && slGetMouseX() <= 287 && slGetMouseY() >= 75 && slGetMouseY() <= 90)
        {
            slSetForeColor(0.0, 0.8, 0.2, 0.5);
            slText(270, 75, "LOOP");
            if(slGetMouseButton(SL_MOUSE_BUTTON_LEFT))
            {
                if (loopTrigger)
                {
                    loopTrigger = 0;
                    Sleep (150);
                }
                else
                {
                    loopTrigger = 1;
                    Sleep (150);
                }
            }
        }

        ///Play
        if (slGetMouseX() >= 132.5  && slGetMouseX() <= 167.5 && slGetMouseY() >= 7.5 && slGetMouseY() <= 42.5)
        {
            slSetForeColor(0.0, 0.8, 0.2, 0.5);
            slTriangleFill(150, 25, 35, 35);
            if(slGetMouseButton(SL_MOUSE_BUTTON_LEFT))
            {
                slSetForeColor(0.0, 0.8, 0.2, 1.0);
                slTriangleFill(150, 25, 35, 35);

                if (fileTrigger)
                {
                    if (!soundTrigger)
                    {
                        slSoundPlay(loadWAV);
                        zeroTime = GetTickCount();
                        soundTrigger  = 1;
                    }
                    else if (pauseTrigger)
                    {
                        slSoundResumeAll();
                        pauseTrigger = 0;
                    }
                }
                else
                    MessageBox (NULL, "Nenhum arquivo foi selecionado.", "Falha", MB_OK);
            }
        }

        ///Stop
        if (slGetMouseX() >= 60  && slGetMouseX() <= 90 && slGetMouseY() >= 8 && slGetMouseY() <= 43)
        {
            slSetForeColor(1.0, 0.0, 0.0, 0.5);
            slRectangleFill(75, 25, 30, 30);
            if(slGetMouseButton(SL_MOUSE_BUTTON_LEFT))
            {
                slSetForeColor(1.0, 0.0, 0.0, 1.0);
                slRectangleFill(75, 25, 30, 30);
                if (soundTrigger)
                {
                    slSoundStopAll();
                    zeroTime = 0;
                    zeroPausedTime = 0;
                    timeElapsed = 0;
                    pausedTime = 0;
                    lastPausedTime = 0;
                    pauseTrigger = 0;
                    soundTrigger = 0;
                }
            }
        }

        ///Pause
        if (slGetMouseX() >= 210  && slGetMouseX() <= 235 && slGetMouseY() >= 8 && slGetMouseY() <= 43)
        {
            slSetForeColor(1.0, 1.0, 0.0, 0.5);
            slRectangleFill(215, 25, 10, 30);
            slRectangleFill(230, 25, 10, 30);
            if(slGetMouseButton(SL_MOUSE_BUTTON_LEFT))
            {
                slSetForeColor(1.0, 1.0, 0.0, 1.0);
                slRectangleFill(215, 25, 10, 30);
                slRectangleFill(230, 25, 10, 30);
                if (soundTrigger)
                {
                    zeroPausedTime = GetTickCount();
                    lastPausedTime += pausedTime;
                    slSoundPauseAll();
                    pauseTrigger = 1;
                }
            }
        }
        slRender();
    }
    slClose();
    return 0;
}

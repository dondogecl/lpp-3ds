/*----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#------  This File is Part Of : ----------------------------------------------------------------------------------------#
#------- _  -------------------  ______   _   --------------------------------------------------------------------------#
#------ | | ------------------- (_____ \ | |  --------------------------------------------------------------------------#
#------ | | ---  _   _   ____    _____) )| |  ____  _   _   ____   ____   ----------------------------------------------#
#------ | | --- | | | | / _  |  |  ____/ | | / _  || | | | / _  ) / ___)  ----------------------------------------------#
#------ | |_____| |_| |( ( | |  | |      | |( ( | || |_| |( (/ / | |  --------------------------------------------------#
#------ |_______)\____| \_||_|  |_|      |_| \_||_| \__  | \____)|_|  --------------------------------------------------#
#------------------------------------------------- (____/  -------------------------------------------------------------#
#------------------------   ______   _   -------------------------------------------------------------------------------#
#------------------------  (_____ \ | |  -------------------------------------------------------------------------------#
#------------------------   _____) )| | _   _   ___   ------------------------------------------------------------------#
#------------------------  |  ____/ | || | | | /___)  ------------------------------------------------------------------#
#------------------------  | |      | || |_| ||___ |  ------------------------------------------------------------------#
#------------------------  |_|      |_| \____|(___/   ------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Licensed under the GPL License --------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Copyright (c) Nanni <lpp.nanni@gmail.com> ---------------------------------------------------------------------------#
#- Copyright (c) Rinnegatamante <rinnegatamante@gmail.com> -------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Credits : -----------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Smealum for ctrulib -------------------------------------------------------------------------------------------------#
#- Aurelio for testing & bug-fixing ------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "include/luaplayer.h"
#include "include/luaGraphics.h"

const char *errMsg;
unsigned char *buffer;

int main(int argc, char **argv)
{
	srvInit();	
	aptInit();
	gfxInit();
	hidInit(NULL);
	irrstInit(NULL);
	fsInit();
	Handle fileHandle;
	u64 size;
	u32 bytesRead;
	int restore;
	
	// Set main script
		char path[256];
		int latest_slash = 0;
		int i=5;
		while (argv[0][i]  != '\0'){
		if (argv[0][i] == '/'){
		latest_slash = i;
		}
		i++;
		}
		strcpy(path,&argv[0][5]);
		path[latest_slash-5] = 0;
		strcat(path,"/index.lua");
		
	while(aptMainLoop())
	{
		restore=0;		
		char error[256];
		
		// Load main script
		FS_path filePath=FS_makePath(PATH_CHAR, path);
		FS_archive script=(FS_archive){ARCH_SDMC, (FS_path){PATH_EMPTY, 1, (u8*)""}};
		FSUSER_OpenFileDirectly(NULL, &fileHandle, script, filePath, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
		FSFILE_GetSize(fileHandle, &size);
		buffer = (unsigned char*)(malloc((size+1) * sizeof (char)));
		FSFILE_Read(fileHandle, &bytesRead, 0x0, buffer, size);
		buffer[size]=0;
		FSFILE_Close(fileHandle);
		svcCloseHandle(fileHandle);
		errMsg = runScript((const char*)buffer, true);
		free(buffer);
		
		if (errMsg != NULL);
                {
				// Fake error to force interpreter shutdown
				if (strstr(errMsg, "lpp_exit_04")){
					break;
				}
				
				strcpy(error,"Error: ");
				strcat(error,errMsg);
				strcat(error,"\n\nPress A to restart\nPress B to exit");
				}

						while (restore==0){
							gspWaitForVBlank();
							RefreshScreen();
							ClearScreen(0);
							ClearScreen(1);
							DebugOutput(error);
							hidScanInput();
							if(hidKeysDown() & KEY_A){
								restore=1;
							}else if(hidKeysDown() & KEY_B){
								restore=2;
							}
							gfxFlushBuffers();
							gfxSwapBuffers();
						}
						if (restore==2){
							break;
						}
	}
	
	fsExit();
	irrstExit();
	hidExit();
	gfxExit();
	aptExit();
	srvExit();

	return 0;
}

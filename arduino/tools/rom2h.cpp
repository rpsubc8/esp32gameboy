//Author: ackerman
//Convert 10 roms .gb to .h Arduino compile gameboy emulate
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#define maxRom 10

unsigned char gb_bufferFile[2248000]; //2 MB y algo

void listFilesRecursively(char *path);
void ShowHelp(void);
void WriteHeadH(char *titleArray, unsigned char totalTitle);
void WriteFileRom(unsigned char num,char *cadPath, char *cadFile);
int GetSizeFile(char *cadFile);
void InitTitles(void);

FILE *gb_fileWrite = NULL;
char gb_titles[maxRom][2048];
unsigned char gb_contRom=0;

//**********************************************
void InitTitles()
{
 for (unsigned char i=0;i<maxRom;i++)
  gb_titles[i][0]='\0';
}

//**********************************************
void ShowHelp()
{
 printf("Author: ackerman\n");
 printf("\n");
 printf(" rom2h directory\n");
 printf("\n");
 printf("Example:\n");
 printf(" rom2h roms");
}

//**********************************************
int GetSizeFile(char *cadFile)
{
 long aReturn=0;
 FILE* fp = fopen(cadFile,"rb");
 if(fp) 
 {
  fseek(fp, 0 , SEEK_END);
  aReturn = ftell(fp);
  fseek(fp, 0 , SEEK_SET);// needed for next read from beginning of file
  fclose(fp);
 }
 return aReturn;
}

//**********************************************
void WriteFileRom(unsigned char num,char *cadPath,char *cadFile)
{
 unsigned char contLine=0;
 FILE *auxWrite = NULL;
 FILE *auxRead = NULL;
 long auxSize=0;
 char cadDest[2048];
 char cadOri[2048];
 sprintf(cadOri,"%s\\%s",cadPath,cadFile);
 sprintf(cadDest,"gbRom%d.h",num);
 auxWrite = fopen(cadDest,"w+");
 if (auxWrite!= NULL)
 {
  fprintf(auxWrite,"//%s\n",cadFile);
  fprintf(auxWrite,"const unsigned char gb_rom_%d_data[] PROGMEM ={\n",num);
  //printf ("Cadori %s\n",cadOri);
  auxRead = fopen(cadOri,"rb");
  if (auxRead!=NULL)
  {
   auxSize = GetSizeFile(cadOri);
   //printf ("Tam %d",auxSize);
   fread(gb_bufferFile,1,auxSize,auxRead);
   for (long i=0;i<auxSize;i++)
   {
    fprintf(auxWrite,"0x%02X",gb_bufferFile[i]);
    if (i<(auxSize-1))
     fprintf(auxWrite,",");
    contLine++;
    if (contLine>15)
    {
     contLine=0;
     fprintf(auxWrite,"\n");
    }
   }
   fclose(auxRead);
  }
  
  fprintf(auxWrite,"};\n");
  fclose(auxWrite);
 }
}

//**********************************************
void WriteHeadH(char *titleArray, unsigned char totalTitle)
{//Cabecera las 10 roms
 if (gb_fileWrite == NULL)
  return;
 fprintf(gb_fileWrite,"#ifndef GB_ROM_H\n");
 fprintf(gb_fileWrite," #define GB_ROM_H\n"); 
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," #define max_rom_gb 10\n");
 fprintf(gb_fileWrite,"\n");
 for (unsigned char i=0;i<maxRom;i++)
 {
  if (strlen(gb_titles[i]) != 0)
   fprintf(gb_fileWrite," #include \"gbRom%d.h\"\n",i);
 }
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," //10 roms title\n");
 fprintf(gb_fileWrite," const char * gb_roms_title[max_rom_gb] PROGMEM={\n");
 for (unsigned char i=0;i<maxRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  \"\"");
  else 
   fprintf(gb_fileWrite,"  \"%d %s\"",i,gb_titles[i]);
  if (i<(maxRom-1))
   fprintf(gb_fileWrite,",\n");
 }
 fprintf(gb_fileWrite,"\n };\n");
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," //10 roms pointer to data\n");
 fprintf(gb_fileWrite," const unsigned char * gb_roms_data[max_rom_gb] PROGMEM={\n");
 for (unsigned char i=0;i<maxRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  NULL");
  else 
   fprintf(gb_fileWrite,"  gb_rom_%d_data",i);
  if (i<(maxRom-1))
   fprintf(gb_fileWrite,",\n");   
 }  
 fprintf(gb_fileWrite,"\n };\n");
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite,"#endif\n");
}

//**********************************************
void listFilesRecursively(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if (!dir)
        return; // Unable to open directory stream
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);
            WriteFileRom(gb_contRom,basePath,dp->d_name);
            int auxLen = strlen(dp->d_name);
            if (auxLen>8)
             auxLen=8;
            strcpy(gb_titles[gb_contRom],dp->d_name);
            gb_titles[gb_contRom][auxLen]='\0';
            gb_contRom++;
            if (gb_contRom > (maxRom-1))
             return;
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            listFilesRecursively(path);
        }
    }
    closedir(dir);
}

//********
//* MAIN *
//********
int main(int argc, char**argv)
{
 char path[100];    //Directory path to list files
 if (argc<2)
 {
  ShowHelp();
  return 0;
 }
 InitTitles();
 gb_fileWrite = fopen("gbrom.h","w+");
 if (gb_fileWrite!= NULL)
 {
  listFilesRecursively(argv[1]);
  WriteHeadH(NULL,10);
  fclose(gb_fileWrite);
 }
  //printf("Enter path to list files: ");    // Input path from user
  //scanf("%s", path);
  //listFilesRecursively(path);
 
 return 0;
}

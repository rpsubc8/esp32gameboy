//Author: ackerman
//Convert roms .gb to .h Arduino compile gameboy emulate
//input
// roms
//  onehour.gb
//output
// dataFlash
//  gbrom.h
//  roms
//   romonehour.h
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#define maxObjects 255
#define max_cad_title 12

unsigned char gb_bufferFile[2248000]; //2 MB y algo

void listFilesRecursively(char *path);
void ShowHelp(void);
void WriteHeadH(char *titleArray, unsigned char totalTitle);
void WriteFileRom(unsigned char num,char *cadPath, char *cadFile);
int GetSizeFile(char *cadFile);
void InitTitles(void);

FILE *gb_fileWrite = NULL;
char gb_titles[maxObjects][32];
char gb_nameFiles[maxObjects][64];
char gb_nameDir[maxObjects][64];
unsigned char gb_contRom=0;

void RemoveExt(char *cad)
{
 int total= strlen(cad);
 if (total > max_cad_title)
 {
  total= max_cad_title;
  cad[max_cad_title]='\0';
 }
 for (int i=0;i<total;i++)
 {
  if (cad[i]==' ')   
   cad[i] = '_';
  else
  {
   if (cad[i]=='.')
   {
    cad[i]='\0';
    return;
   }
  }
 }
}


//**********************************************
void InitTitles()
{
 for (unsigned char i=0;i<maxObjects;i++)
  gb_titles[i][0]='\0';
}

//**********************************************
void ShowHelp()
{
 printf("Author: ackerman\n\n");
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
void WriteFileROM(unsigned char num,char *cadPath,char *cadFile, char *cadFileSource)
{
 unsigned char contLine=0;
 FILE *auxWrite = NULL;
 FILE *auxRead = NULL;
 long auxSize=0;
 char cadDest[2048];
 char cadOri[2048];
 sprintf(cadOri,"%s\\%s",cadPath,cadFileSource);
 sprintf(cadDest,"output/dataFlash/roms/rom%s.h",cadFile);
 auxWrite = fopen(cadDest,"w+");
 if (auxWrite!= NULL)
 {
  fprintf(auxWrite,"#ifndef _ROM_%s_H\n",cadFile);
  fprintf(auxWrite," #define _ROM_%s_H\n",cadFile);     
  //WriteHexData(auxWrite,cadFile);  

  auxRead = fopen(cadOri,"rb");
  if (auxRead!=NULL)
  {
   auxSize = GetSizeFile(cadOri);
   
   fprintf(auxWrite," //rom %s %d bytes\n\n",cadFile,auxSize);
   fprintf(auxWrite,"const unsigned char gb_rom_%s[]={\n",cadFile);
      
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
  
  
  fprintf(auxWrite,"\n};\n");
  fprintf(auxWrite,"#endif\n");
  fclose(auxWrite);
 }
}


//**********************************************
void WriteHeadROM_H(char *cadDefine)
{//Los 48k
 if (gb_fileWrite == NULL)
  return;
 fprintf(gb_fileWrite,"#ifndef %s\n",cadDefine);
 fprintf(gb_fileWrite," #define %s\n",cadDefine); 
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," //#include <stddef.h>\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) != 0)
   fprintf(gb_fileWrite," #include \"roms/rom%s.h\"\n",gb_titles[i]);
 }
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," #define max_list_rom %d\n\n\n",gb_contRom);
 fprintf(gb_fileWrite," //roms\n //Titulos\n");
 fprintf(gb_fileWrite," static const char * gb_list_rom_title[max_list_rom]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  \"\"");
  else 
   fprintf(gb_fileWrite,"  \"%s\"",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");
 }
 fprintf(gb_fileWrite,"\n };\n");
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," //Datos rom\n");
 fprintf(gb_fileWrite," static const unsigned char * gb_list_rom_data[max_list_rom]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  NULL");
  else 
   fprintf(gb_fileWrite,"  gb_rom_%s",gb_titles[i]);
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");   
 }  
 fprintf(gb_fileWrite,"\n };\n");
 
 fprintf(gb_fileWrite,"\n");
 //fprintf(gb_fileWrite,"#endif\n");
}


//********************************************
void WriteSizeROM()
{
 char cadDestino[1024];
 int auxSize;
 if (gb_fileWrite == NULL)
  return;
 fprintf(gb_fileWrite," //Tamanio en bytes\n");
 fprintf(gb_fileWrite," //static const int gb_list_rom_size[max_list_rom]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  //\"\"");
  else
  {
   sprintf(cadDestino,"input\\roms\\%s",gb_nameFiles[i]);   
   auxSize = GetSizeFile(cadDestino);
   fprintf(gb_fileWrite," // %d",auxSize);
  }
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,"//,\n");
 }
 fprintf(gb_fileWrite,"\n //};\n");
 fprintf(gb_fileWrite,"\n");  
}



//**********************************************
void listFilesRecursively(char *basePath)
{     
    char cadFileSource[1000];
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
            
            strcpy(cadFileSource,dp->d_name);
            strcpy(gb_nameFiles[gb_contRom],dp->d_name); //Nombre completo
            RemoveExt(dp->d_name);
            strcpy(gb_titles[gb_contRom],dp->d_name);

            WriteFileROM(gb_contRom,basePath,dp->d_name,cadFileSource);            
                        
            gb_contRom++;
            if (gb_contRom > (maxObjects-1))
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

//*************************************************
void ProcesaFicherosROM()
{
 gb_contRom=0;
 gb_fileWrite = fopen("output/dataFlash/gbrom.h","w+");
 if (gb_fileWrite!= NULL)
 {
  listFilesRecursively("input/roms");
  WriteHeadROM_H("_GB_ROM_H");
  WriteSizeROM();
  fprintf(gb_fileWrite,"#endif\n");
  fclose(gb_fileWrite);
 }     
}



//********
//* MAIN *
//********
int main(int argc, char**argv)
{
 char path[100];    //Directory path to list files
 ShowHelp();

 InitTitles();
 
 ProcesaFicherosROM();
 //printf("Enter path to list files: ");    // Input path from user
 //scanf("%s", path);
 //listFilesRecursively(path);
 
 return 0;
}

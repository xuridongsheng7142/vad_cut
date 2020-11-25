#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include "INS.h"
#include "IVad.h"
#include "WavReader.h"
#include "WavWriter.h"

int main_test_wav_cut(int argc, char *argv[])
{
#define NN 160
#define Cut_Samples_length (65*16000)
#define Cut_Location (60*16000)
#define Cut_Time 1
	char input_path[128];
	memset(input_path, 0, sizeof(input_path));
        strcpy(input_path, argv[0]);
	FILE *file;
	if( (file = fopen(input_path, "rb") )== NULL)
	{
	    printf("Input File Error.\n");
            return 1;   
	}	
	fclose(file);
#if 0
        if(access(argv[1], 0) == -1)
        {
            printf("Output Path Error.\n");
            return 1;
        }	
#endif
	printf("%s\n", argv[0]);
	char file_format[5];
	for(int i= strlen(input_path) - 4; i<strlen(input_path); i++)
	{
	    
	    file_format[i - strlen(input_path) + 4] = input_path[i];
	}

	file_format[4] = '\0';
        printf("%s\n", file_format);

	if(strcmp(file_format,".wav") != 0)
	{
	    printf("File Format Error.\n");
	    return 1;
	}
	


        WavReader wav_reader;
        wav_reader.Open(argv[0]);
        int channel = wav_reader.m_nChannels;
	int NsampleRate = wav_reader.m_nSampleRate;
	int bits_per_sample = wav_reader.m_nBitsPerSample;
	int mic_samples = 0,size = 0;
        short *mic_input_pt = NULL;
 
        if (((wav_reader.m_nBitsPerSample >> 3) * channel) == 0)
        {
                printf("wav Read Error\n");
                return 1;
        }
        else
	{
            mic_samples = wav_reader.m_nLength / ((wav_reader.m_nBitsPerSample>>3) * channel);
            size = wav_reader.m_nLength;
            mic_input_pt = (short *)malloc(size);
            memset(mic_input_pt,0,size);
            wav_reader.Read(mic_input_pt, mic_samples*channel);
            wav_reader.Close();
	}

	int cut_times = mic_samples/Cut_Samples_length;
	int frames = mic_samples/NN;

	printf("mic_samples = %d \n", mic_samples);
	printf("frames = %d\n", frames);
	printf("cut times = %d\n", cut_times);	

        WavWriter wav_write;

	char str1[100];
        char const *str2 = ".wav";
        char wav_out[128];
	
        memset(wav_out, 0, sizeof(wav_out));
        strcpy(wav_out, argv[1]);
        int len = strlen(wav_out);
	wav_out[len] = '/';
        char tmp[128];
        int name_len = 0;
	for(int i = strlen(input_path) - 5; i>0; i--)
        {
            if( input_path[i] != '/' )
            {
               tmp[name_len] = input_path[i];
               name_len++;
            }
            else
            {
               for(int k = 0; k<name_len; k++)
               {
                   wav_out[len + k + 1] = tmp[name_len - k -1];
               }
	  	break;
            }
        }
 

	len = strlen(wav_out);
	wav_out[len] = '\0';
	printf("1---%s\n", wav_out);

	if(cut_times > 0)
	{
//NS Init
            void *pNS_inst = NULL;
            int ns_Mode = 3;
	    int vad_Mode = 2;
	    int sampleRate = 16000;
	    fns_new(&pNS_inst, sampleRate);
            fns_set_policy(pNS_inst, ns_Mode);
//VAD Init
	    void *pVad_inst = NULL;
	    int vad = 3;
	    fvad_new(&pVad_inst, sampleRate, vad_Mode);
//Cut Process
	    printf("-----Init End-----\n");
	    int start = 0, end = 0, loops = 0;
	    int cut_num = 0;
	    int max_loop = 500;
	    short mic_frame[NN];
	    short out_frame[NN];

	    WavWriter wav_write;
	    
	    int steps = Cut_Location;
	
	    while(cut_times != 0)
	    {
	       start = steps + loops* NN;
	       end = steps + (loops+1)*NN;
		
	       for(int i = start; i < end; i ++) {
                     mic_frame[i - start] = mic_input_pt[i];
               }
	       fns_process(pNS_inst, mic_frame, out_frame);
	       vad = fvad_process(pVad_inst, out_frame, NN);
	       
	       if( (vad == 0) || (loops > max_loop))
	       {
		   printf("-----VAD = 0 or loop > max-----\n");
	
		   snprintf(str1, sizeof(str1), "%d", cut_num);
		   cut_times--;
		   wav_out[len] = '_';
		   if(cut_num<10)
		   {
		       wav_out[len + 1] = '0';
		       wav_out[len + 2] = '\0';
		       strcat(wav_out, str1);
                       wav_out[strlen(str1)+len+2] = '\0';
                       strcat(wav_out, str2);
                       wav_out[strlen(str1)+len+4+2] = '\0';
		   }
		   else
		   {
		       wav_out[len + 1] = '\0';
	 	       strcat(wav_out, str1);
	 	       wav_out[strlen(str1)+len+1] = '\0';
	 	       strcat(wav_out, str2);
	 	       wav_out[strlen(str1)+len+4+1] = '\0';
		   }
	 	   printf("%s\n", wav_out);
//save wav
   		  short *output_cut_pt = NULL;
		  output_cut_pt = (short *)malloc(sizeof(short)*(end - steps + Cut_Location));
	  
                 for(int i = steps - Cut_Location; i < end; i ++) {
                     output_cut_pt[i-(steps - Cut_Location)] = mic_input_pt[i];
                  }

		  printf("End Samples Location = %d\n",end);

         	  wav_write.Open(wav_out, 16000, bits_per_sample, channel);
	          wav_write.Write(output_cut_pt, (end - steps + Cut_Location)*channel);
//reset
         	  wav_write.Close();
		  wav_out[len] = '\0';
		  cut_num++;
		  steps = end + Cut_Location;
		  loops = 0;  
		  free(output_cut_pt);
		  output_cut_pt = NULL;

		  if(cut_times == 0)
		  {
			cut_times = (mic_samples - end)/Cut_Samples_length;
		  }
	       }
	       else
	       {
		   loops ++;
	       }
	    }


//Last write
	   wav_out[len] = '_';
           snprintf(str1, sizeof(str1), "%d", cut_num);
	   if(cut_num<10)
           {
              wav_out[len + 1] = '0';
              wav_out[len + 2] = '\0';
              strcat(wav_out, str1);
              wav_out[strlen(str1)+len+2] = '\0';
              strcat(wav_out, str2);
              wav_out[strlen(str1)+len+4+2] = '\0';
           }
           else
           {

  	      wav_out[len+1] = '\0';
              strcat(wav_out, str1);
              wav_out[strlen(str1)+len+1] = '\0';
              strcat(wav_out, str2);
              wav_out[strlen(str1)+len+4+1] = '\0';
	   }
           printf("%s\n", wav_out);

//Save Wav
           short *output_cut_pt = NULL;
           output_cut_pt = (short *)malloc(sizeof(short)*(mic_samples - end));

           for(int i = end; i < mic_samples; i ++) {
               output_cut_pt[i - end] = mic_input_pt[i];
           }

           wav_write.Open(wav_out, 16000, bits_per_sample, channel);
           wav_write.Write(output_cut_pt, (mic_samples - end)*channel);
           wav_write.Close();
 //free
	   fns_free(pNS_inst);
	   fvad_free(pVad_inst);
	   printf("Cut\n");
	}
	else
	{

            strcat(wav_out, str2);
            wav_out[strlen(str1)+len+4] = '\0';
            printf("%s\n", wav_out);
	    wav_write.Open(wav_out, 16000, bits_per_sample, channel);
            wav_write.Write(mic_input_pt, mic_samples*channel);
            wav_write.Close();
	    printf("No Cut\n");
	}
	
	free(mic_input_pt);
	mic_input_pt = NULL;
	return 0;
}

int main(int argc, char* argv[])
{
    int res;
    if(argc == 3)
    {
        res = main_test_wav_cut(argc-1, &argv[1]);
        if(res == 0)
        {
	    printf("Cut Success\n");
        }
        else
        {
	    printf("Cut Error\n");
        }
    }
    else
    {
	printf(" Input Param Error. Use info:\n");
	printf("./AudioTest input_file_path output_path\n");
    }

    return 0;
}

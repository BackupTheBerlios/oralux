#ifndef YASR
#define YASR 1

void buildConfigurationYasr(struct textToSpeechStruct* theTextToSpeech);
void runYasr( struct textToSpeechStruct* theTextToSpeech, 
	      enum language theMenuLanguage,
	      char* theCommand);
#endif


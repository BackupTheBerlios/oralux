#ifndef YASR
#define YASR 1

void buildConfigurationYasr(struct textToSpeechStruct* theTextToSpeech);
void runYasr( enum textToSpeech theTextToSpeech, enum language theMenuLanguage, char* theCommand);

#endif


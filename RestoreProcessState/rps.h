#ifndef rps_h
#define rps_h

void rpsInitialize();
void rpsSaveState(const char *path_to_outfile);
void rpsLoadState(const char *path_to_infile);

#endif // rps_h

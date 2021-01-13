#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#define q	11		    /* for 2^11 points */
#define N	(1<<q)		/* N-point FFT, iFFT */

typedef float real;
typedef struct {
	real Re;
	real Im;
} complex;

#ifndef PI
# define PI	3.14159265358979323846264338327950288
#endif

complex v[N], scratch[N];

void fft(complex *v, int n, complex *tmp) {
	if (n > 1) { /* otherwise, do nothing and return */
		int k, m;
		complex z, w, *vo, *ve;
		ve = tmp;
		vo = tmp + n / 2;
		for (k = 0; k < n / 2; k++) {
			ve[k] = v[2 * k];
			vo[k] = v[2 * k + 1];
		}
		fft(ve, n / 2, v); /* FFT on even-indexed elements of v[] */
		fft(vo, n / 2, v); /* FFT on odd-indexed elements of v[] */
		for (m = 0; m < n / 2; m++) {
			w.Re = cos(2 * PI * m / (double) n);
			w.Im = -sin(2 * PI * m / (double) n);
			z.Re = w.Re * vo[m].Re - w.Im * vo[m].Im; /* Re(w*vo[m]) */
			z.Im = w.Re * vo[m].Im + w.Im * vo[m].Re; /* Im(w*vo[m]) */
			v[m].Re = ve[m].Re + z.Re;
			v[m].Im = ve[m].Im + z.Im;
			v[m + n / 2].Re = ve[m].Re - z.Re;
			v[m + n / 2].Im = ve[m].Im - z.Im;
		}
	}
	return;
}

void *fft_wrapper() {
	int i, m;
	float abs[N];
	fft(v, N, scratch);
	int minIdx = (0.5 * 2048) / 50; // position in the PSD of the spectral line corresponding to 30 bpm
	int maxIdx = 3 * 2048 / 50; // position in the PSD of the spectral line corresponding to 180 bpm
	for (i = 0; i < N; i++) {
		abs[i] = (50.0 / 2048) * ((v[i].Re * v[i].Re) + (v[i].Im * v[i].Im));
	}

	m = minIdx;
	for (i = minIdx; i < (maxIdx); i++) {
		if (abs[i] > abs[m])
			m = i;
	}

	printf("\n\n\n%d bpm\n\n\n", (m) * 60 * 50 / 2048);
}

int main(int argc, char **argv) {
	char *app_name = argv[0];
	char *dev_name = "/dev/mymod_dev";
	int fd = -1;
	int value, k, m, i;
	struct timeval start, stop;

	pthread_t thread_fft;

	if ((fd = open(dev_name, O_RDWR)) < 0) {
		fprintf(stderr, "%s: unable to open %s: %s\n", app_name, dev_name,
				strerror(errno));
		return (1);
	}

	while (1) {
		fprintf(stdout, "Reading...\n");
		for (k = 0; k < N; k++) {
			gettimeofday(&start, NULL);
			read(fd, &value, sizeof(int));
			v[k].Re = value;
			v[k].Im = 0;

			if (k == N - 1) {
				pthread_create(&thread_fft, NULL, fft_wrapper, NULL);
				pthread_detach(thread_fft);
			}
			gettimeofday(&stop, NULL);
			usleep(20000 - (stop.tv_usec - start.tv_usec));
		}
	}

	close(fd);
	exit(EXIT_SUCCESS);
}

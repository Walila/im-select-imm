#include "stdafx.h"
#include "parg.h"
#include <cstdlib>
#include <cstring>
#include <Windows.h>
#include <immdev.h>

using namespace std;

int g_verbose = 0;
int g_shift_mode = 0;

int getInputMethod() {
	HWND hwnd = GetForegroundWindow();
	if (hwnd) {
		DWORD threadID = GetWindowThreadProcessId(hwnd, NULL);
		HKL currentLayout = GetKeyboardLayout(threadID);
		unsigned int x = (unsigned int)currentLayout & 0x0000FFFF;
		if (g_verbose) {
			printf("[verbose] getInputMethod: HWND=0x%p threadID=%lu HKL=0x%p locale=0x%04X\n",
			       (void*)hwnd, threadID, (void*)currentLayout, x);
		}
		return ((int)x);
	}
	if (g_verbose) {
		printf("[verbose] getInputMethod: GetForegroundWindow returned NULL\n");
	}
	return 0;
}

void switchInputMethod(int locale) {
    if (locale < 0) {
        return;
    }
	HWND hwnd = GetForegroundWindow();
	LPARAM currentLayout = ((LPARAM)locale);
	if (g_verbose) {
		printf("[verbose] switchInputMethod: HWND=0x%p locale=0x%04X\n",
		       (void*)hwnd, locale);
	}
	PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, currentLayout);
}

// API: https://learn.microsoft.com/en-us/previous-versions/aa913780(v=msdn.10)
// For  Microsoft Old Chinese IME(Win10 and Previous) :
//       0: English
//       1: Chinese
// For  Microsoft New Chinese IME(Win11) :
//       0: English / Half Shape
//       1: Chinese / Half Shape
//       1024: English / Full Shape (Bit10 and Bit1 used)
//       1025: Chinese / Full Shape
LRESULT getInputMode(){
    HWND foregroundWindow  = GetForegroundWindow();
    HWND foregroundIME = ImmGetDefaultIMEWnd(foregroundWindow);
    if(foregroundIME){
        LRESULT result = SendMessage(foregroundIME, WM_IME_CONTROL, 0x001, 0);
        if (g_verbose) {
            printf("[verbose] getInputMode(IMM32): HWND=0x%p IME_HWND=0x%p mode=%lld\n",
                   (void*)foregroundWindow, (void*)foregroundIME, (long long)result);
        }
        return result;
    } else {
        if (g_verbose) {
            printf("[verbose] getInputMode(IMM32): ImmGetDefaultIMEWnd returned NULL\n");
        }
        return 0;
    }
}

void switchInputMode(LRESULT mode){
    if ( mode < 0 ) {
        return;
    }

    if (g_shift_mode) {
        // -s flag: Simulate Shift key to toggle Chinese/English mode.
        // Required for pure TSF IMEs (e.g. Boshiamy J) that fake-accept
        // IMM32 writes and corrupt subsequent reads.
        LRESULT preMode = getInputMode();

        if (g_verbose) {
            printf("[verbose] switchInputMode(Shift): current_mode=%lld target_mode=%lld\n",
                   (long long)preMode, (long long)mode);
        }

        if (preMode == mode) {
            if (g_verbose) {
                printf("[verbose] switchInputMode(Shift): mode already correct, nothing to do\n");
            }
            return;
        }

        INPUT inputs[2] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_SHIFT;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = VK_SHIFT;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        UINT sent = SendInput(2, inputs, sizeof(INPUT));
        if (g_verbose) {
            printf("[verbose] switchInputMode(Shift): SendInput (sent=%u)\n", sent);
        }
    } else {
        // Default: IMM32 IMC_SETCONVERSIONMODE via SendMessage.
        // Works for standard Microsoft IMEs.
        HWND foregroundWindow = GetForegroundWindow();
        HWND foregroundIME = ImmGetDefaultIMEWnd(foregroundWindow);

        if (g_verbose) {
            printf("[verbose] switchInputMode(IMM32): HWND=0x%p IME_HWND=0x%p target_mode=%lld\n",
                   (void*)foregroundWindow, (void*)foregroundIME, (long long)mode);
        }

        if (foregroundIME) {
            SendMessage(foregroundIME, WM_IME_CONTROL, IMC_SETCONVERSIONMODE, (LPARAM)mode);
        }
    }
}


int main(int argc, char** argv)
{
    // init parg
    struct parg_state ps;
    int c;
    parg_init(&ps);
    // h: help page
    // s: use Shift key simulation for mode switch
    // d: delay INT ms
    // v: verbose output
    const char optstring[] = "hsvd:" ;
    int optend = parg_reorder(argc, argv, optstring, NULL);

    int delay = 30 ; // ms
    while ((c = parg_getopt(&ps, optend, argv, optstring)) != -1) {
        switch (c) {
            case 'h':
                printf( \
                        "USAGE:                                              \n" \
                        "       im-select-imm [-h] [-s] [-v] [-d DELAY] [METHOD] [MODE]\n" \
                        "OPTIONS:                                            \n" \
                        "       -s  Use Shift key simulation for mode switch \n" \
                        "           (for pure TSF IMEs like Boshiamy J)      \n" \
                        "           Default: IMM32 IMC_SETCONVERSIONMODE     \n" \
                        "VERSION:                                            \n" \
                        "       1.2.0                                        \n" \
                        );
                return 0;
            case 's':
                g_shift_mode = 1;
                break;
            case 'v':
                g_verbose = 1;
                break;
            case 'd':
                delay = atoi(ps.optarg);
                break;
            case 1:
                // for remaining option
                break;
        }
    }

    // process position args

    int remian_argc = argc - ps.optind ;
    char **remain_argv = argv + ps.optind ;

    if (g_verbose) {
        printf("[verbose] argc=%d optind=%d remain=%d delay=%d\n",
               argc, ps.optind, remian_argc, delay);
    }

    // get mode
    if ( remian_argc == 0 ) {
        int imID = getInputMethod();
        int imMode = getInputMode();
        printf("%d-%d\n", imID, imMode);
        return 0;
    }

    // not getmode, so is set mode
    LRESULT mode = -1;
    int method = -1;
    if(  remian_argc == 1 ) {
        char *dash_p = strchr(remain_argv[0],'-');
        if(dash_p){
            char locale_str[16];
            memccpy(locale_str,remain_argv[0],'-',sizeof locale_str);

            method = atoi(locale_str);
            mode = atoi(dash_p + 1);
        } else {
            method = atoi(remain_argv[0]);
        }
    }

    if ( remian_argc == 2 ) {
		// im-select-imm [Method] [Mode]
        method = atoi(remain_argv[0]);
        mode = atoi(remain_argv[1]);
    }

    if (g_verbose) {
        printf("[verbose] SET METHOD: %d MODE: %lld\n", method, (long long)mode);
    }

    switchInputMethod(method);
    Sleep(delay);
    switchInputMode(mode);

	return 0;
}

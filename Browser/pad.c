

#define ROM_PADMAN
#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif
#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif

static char padBuf[256] __attribute__((aligned(64)));
static char actAlign[6];
static int actuators;

int waitPadReady(int port, int slot)
{
	int state;
	int lastState;
	char stateString[16];

	state = padGetState(port, slot);
	lastState = -1;
	while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
		if (state != lastState) {
			padStateInt2String(state, stateString);
			printf("Please wait, pad(%d,%d) is in state %s ", 
						port, slot, stateString);
		}
		lastState = state;
		state=padGetState(port, slot);
	}
	// Were the pad ever 'out of sync'?
	if (lastState != -1) {
		printf("Pad OK! ");
	}
	return 0;
}

int initializePad(int port, int slot)
{

	int ret;
	int modes;
	int i;

	waitPadReady(port, slot);

	// How many different modes can this device operate in?
	// i.e. get # entrys in the modetable
	modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
	printf("The device has %d modes ", modes);

	if (modes > 0) {
		printf("( ");
		for (i = 0; i < modes; i++) {
			printf("%d ", padInfoMode(port, slot, PAD_MODETABLE, i));
		}
		printf(")");
	}

	printf("It is currently using mode %d ", 
				padInfoMode(port, slot, PAD_MODECURID, 0));

	// If modes == 0, this is not a Dual shock controller 
	// (it has no actuator engines)
	if (modes == 0) {
		printf("This is a digital controller? ");
		return 1;
	}

	// Verify that the controller has a DUAL SHOCK mode
	i = 0;
	do {
		if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
			break;
		i++;
	} while (i < modes);
	if (i >= modes) {
		printf("This is no Dual Shock controller ");
		return 1;
	}

	// If ExId != 0x0 => This controller has actuator engines
	// This check should always pass if the Dual Shock test above passed
	ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
	if (ret == 0) {
		printf("This is no Dual Shock controller?? ");
		return 1;
	}

	printf("Enabling dual shock functions ");

	// When using MMODE_LOCK, user cant change mode with Select button
	padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

	waitPadReady(port, slot);
	printf("infoPressMode: %d ", padInfoPressMode(port, slot));

	waitPadReady(port, slot);		
	printf("enterPressMode: %d ", padEnterPressMode(port, slot));

	waitPadReady(port, slot);
	actuators = padInfoAct(port, slot, -1, 0);
	printf("# of actuators: %d ",actuators);

	if (actuators != 0) {
		actAlign[0] = 0;	// Enable small engine
		actAlign[1] = 1;	// Enable big engine
		actAlign[2] = 0xff;
		actAlign[3] = 0xff;
		actAlign[4] = 0xff;
		actAlign[5] = 0xff;

		waitPadReady(port, slot);
		printf("padSetActAlign: %d ", 
					padSetActAlign(port, slot, actAlign));
	}
	else {
		printf("Did not find any actuators. ");
	}

	waitPadReady(port, slot);

	return 1;
}

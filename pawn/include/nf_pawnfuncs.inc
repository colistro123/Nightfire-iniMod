ConsoleMessage(...) {
	new string[256], tempstr[128];
    for(new i=0; i<numargs(); i++) {
		strformat(tempstr, sizeof(tempstr), false, "%s", getarg(i, i));
		strcat(string, tempstr);
	}
	print(string);
}


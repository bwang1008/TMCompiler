bool nothing() {
	printInt(-2);
	printSpace();
	return true;
}

int main() {
	bool b = false;

	//b = (b && nothing());
	b &= nothing();
	
	int ans = 3;
	if(b) {
		ans = 5;
	}

	return ans;
}

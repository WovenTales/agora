// Contained in namespace Database

private:

template <typename T>
void clearStaged(std::queue<T*> &stage) {
	while (!stage.empty()) {
		delete stage.front();
		stage.pop();
	}
};

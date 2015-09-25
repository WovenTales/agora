// Contained in namespace Database

private:

//! Discard unsaved changes from a stage
/*! \tparam T type contained by the stage to clear
 *
 *  \param stage the stage to clear
 */
template <typename T>
void clearStaged(std::queue<T*> &stage) {
	while (!stage.empty()) {
		delete stage.front();
		stage.pop();
	}
};

// Contained in namespace Database

private:

//! Discard unsaved changes from a stage
/*! \todo Document template parameters
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

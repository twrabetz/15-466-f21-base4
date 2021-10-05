#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include "TextRenderer.hpp"
#include "data_path.hpp"

struct OptionNode;

struct StoryNode
{
	int level;
	std::string title;
	std::string body;
	std::vector<OptionNode> options;
};

struct OptionNode
{
	std::string label;
	StoryNode* targetNode;
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	void activate_node(StoryNode* node);

	StoryNode* root;
	StoryNode* currentNode;

	glm::vec3 white = glm::vec3(1.0f);

	glm::vec3 green = glm::vec3(0.2f, 1.0f, 0.1f);
	glm::vec3 lime = glm::vec3(0.5f, 1.0f, 0.1f);
	glm::vec3 yellow = glm::vec3(0.8f, 1.0f, 0.1f);

	TextRenderer titleRenderer = TextRenderer(data_path("CMinoanHinted.ttf"), 100, 0.0f, 0.0f, 800.0f, 1920.0f, true, nullptr, false, white);
	TextRenderer bodyOverflow = TextRenderer(data_path("LinguisticsPro-Regular.otf"), 36, 1190.0f, 125.0f, 500.0f, 500.0f, false, nullptr, true, white);
	TextRenderer bodyRenderer = TextRenderer(data_path("LinguisticsPro-Regular.otf"), 36, 250.0f, 125.0f, 500.0f, 500.0f, false, &bodyOverflow, false, white);

	TextRenderer psychBodyOverflow2 = TextRenderer(data_path("Karma-Regular.otf"), 36, 1300.0f, 500.0f, 350.0f, 350.0f, false, nullptr, true, yellow);
	TextRenderer psychBodyOverflow = TextRenderer(data_path("Karma-Regular.otf"), 36, 800.0f, 300.0f, 350.0f, 350.0f, false, &psychBodyOverflow2, true, lime);
	TextRenderer psychBodyRenderer = TextRenderer(data_path("Karma-Regular.otf"), 36, 300.0f, 100.0f, 350.0f, 350.0f, false, &psychBodyOverflow, false, green);

	TextRenderer optionRenderer = TextRenderer(data_path("Now-Regular.otf"), 42, 0.0f, 800.0f, 1500.0f, 100.0f, true, nullptr, false, white);

	TextRenderer* currentBodyRenderer = &bodyRenderer;

	StoryNode* visions;

};

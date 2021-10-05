#include "PlayMode.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

#include <iostream>

PlayMode::PlayMode() {

	StoryNode* church = new StoryNode{ 10, "Church", "The pastor drones on and on in the 90-degree weather. \"Can you believe Jeremiah has "
		"FOUR wives?!\" whispers Constance to Veronica...\n\nEverybody stands up. Time to sing songs.", {{"Hallelujah", nullptr}}};
	StoryNode* mormonism = new StoryNode{ 10, "Mormonism", "You come home with groceries. All the chores are done. Finally, time for a little TV..."
		"\n\n\"Don't be silly darling! It's time for Church!\" exclaims Sheila.\n\n\nAh right, church.", {{"Relapse", nullptr}, {"Church", church}} };
	church->options[0].targetNode = mormonism;

	StoryNode* praiseJesus = new StoryNode{ 9, "Hallelujah!", "Years pass. You are now married to Sheila. And Constance. And Veronica.\n\n"
		"Yippee.", {{"Relapse", nullptr}, {"Mormonism", mormonism}}};

	StoryNode* giveInToResponsibleness = new StoryNode{ 8, "Latter Day Chores", "You let the responsibleness flow through you...\n\n"
		"Over the next few weeks, changes you never imagined begin to take place. You finish your homework! You graduate from college.\nBut the "
		"responsibleness doesn't stop there! One day, you pass by a mormon handing out leaflets. \"Become a mormon and praise Jesus!\" he exclaims."
		"\n\n\n\nYou have been pretty responsible lately. But is this really you?", {{"Relapse", nullptr}, {"Praise Jesus", praiseJesus}}};
	StoryNode* resistResponsibleness = new StoryNode{ 8, "The #Resistance", "You successfully beat back the responsibleness. Wow, that was scary. "
		"Anyhow, you spend "
		"the rest of the day watching TV. And the next day. And the next day....", { {"Zzzzzzz....", nullptr} } };

	StoryNode* cleanEverything = new StoryNode{ 7, "Clean Everything!", "You clean everything, which takes freaking forever.\n\nAfterwards, "
		"Sheila is pleased. For some "
		"reason, you are pleased as well. You sense a sudden wave of responsibleness washing over you!", 
		{{"Resist!", resistResponsibleness}, {"Give In", giveInToResponsibleness}}};
	StoryNode* breakUrWord = new StoryNode{ 7, "Betrayal", "Sheila goes to do taekwondo drills. Instead of cleaning everything, "
		"you go pee and then watch TV.\n\n"
		"Two hours later, Sheila comes back and finds twice as much trash as before.\n\nShe explodes into a rage like you've never seen.\nThere's "
		"no escape now- it's time for big smack. You are knocked out cold like a chump.", {{"Sleep It Off", nullptr}} };

	StoryNode* negotiate = new StoryNode{ 6, "Negotiation", "You agree to clean everything.\n\nSheila agrees not to wreck you.", 
		{{"Clean Everything", cleanEverything}, {"Go Back On Ur Word", breakUrWord}}};
	//MORMONISM ARC

	StoryNode* marriage = new StoryNode{ 10, "The Wedding", "You have a beautiful wedding attended by both your families, who are now giant caterpillars. "
		"\n\nWhat a wonderful life!\n\n[This is an ending. Quit game to restart]." };

	StoryNode* hospital = new StoryNode{ 9, "The Hospital", "You come to in a hospital bed. Sheila is sleeping beside you in a chair. As you come to, "
		"she wakes up and tearfully embraces you. \"I thought you were gonna die because I smacked you too hard!\n\n\nIt made me realize that I can't live "
		"without you!\"\n\nOh jeez.", {{"Marry Her", marriage}, {"Back To Sleep", nullptr}} };

	StoryNode* glassField = new StoryNode{ 8, "The Hills of Glass", "You emerge from the vortex and land hard on a smooth surface. All around you,"
		" undulating hills of rainbow-flecked glass stretch endlessly to the horizon. Two suns float above you in the sky. Huge raindrops descend from"
		" above, hardening as they fall until they impact the vast landscape, generating a symphony of teardrop notes. The drops roll down and "
		"accumulate unharmed into deep mounds...", { {"Splushhh", hospital}}};

	visions = new StoryNode{ 7, "Visions...", "As the smack collides with your face, the kitchen dissolves into colored ribbons around you."
		" In the black void, a chrome refrigerator opens its door and swallows you whole. Gigantic onions and peppers gyrate around you, falling with you "
		" towards a technicolor vortex. Cloves of garlic emerge from your nose and your hands turn into potatoes. As the vortex absorbs you, you feel "
		" the simple satisfaction of being a turnip, comfortable in the warm soil around you. Suddenly, a farmer grasps your leaves and tries to harvest you. "
		"Your roots cling desperately to the soil as he pulls...", { {"Nghhhhhh", glassField}} };

	StoryNode* acceptFate = new StoryNode{ 6, "The Big Smack", "Sheila's outstretched palm approaches you in slow motion. As death looms over you, "
		"your trash life flashes before your eyes. None of it is memorable.\n\nA strange sense of calm envelops you. This is what you deserve. "
		"\nby accepting death, you move beyond it.", {{"SMACK!", visions}}};
	//BAD TRIP ARC

	StoryNode* resume = new StoryNode{ 12, "CONVERSATIONAL SPANISH", "CONVERSATIONAL. SPANISH.\n\nThese two words - the only ones on your resume - "
		"stare the shocked teller in the face. With trembling hands, he presses a button under his desk. A wall panel to your left opens to reveal a "
		"hidden elevator.\n\n\"G-g-g-o right ahead, sir!\" stammers the teller, saluting you. You step inside the elevator, and press the solitary "
		"TOP button. You are whisked up hundreds of floors to the master boardroom. As you stride out of the elevator, the bank CEO hands you his "
		" resignation and the gray-suited executives bow down, chanting,\n\n\"ALL HAIL LORD BUSINESS!\"\n\"ALL HAIL LORD BUSINESS!\"\n"
		"[This is an ending. Quit the game to restart.]" };
	StoryNode* withdrawal = new StoryNode{ 12, "Withdrawal", "\"Here you go sir- $50.\"", {{"Back To Business", nullptr}} };

	StoryNode* bank = new StoryNode{ 11, "The Bank", "You fling wide the doors of your local bank and rush up to the front desk. The teller looks up "
		"at you with mild surprise. Time to get this bread!", {{"Make A Withdrawal", withdrawal}, {"Show Them Your Resume", resume}} };
	withdrawal->options[0].targetNode = bank;

	StoryNode* combover = new StoryNode{ 10, "Your Final Form!", "With trembling hands, you do the terrible deed. Looking up at the mirror, you see "
		"that you've reached your final form. Thanks to the power of your combover, you are finally a high-powered go-getter. Time to head on over "
		"to get a job at the bank!", {{"Business!", bank}} };
	StoryNode* endItAll = new StoryNode{ 10, "To Be Continued?", "Slowly, you raise your hand towards the mirror. Your beam will be reflected and end "
		"your pathetic existence. Deep down, you know it's better this way. You gather your inner strength and release it one last time...\n\n\n"
		"What's this? Your plots are so dumb they start bleeding into real life!\n\nThanks to the power of \"Mirror Crystals\", your beam punches a hole "
		"through time! What will happen next?\n Stay tuned for the next episode!", { {"Cowabunga!", nullptr}} };

	StoryNode* pursueMoney = new StoryNode{ 9, "Money!", "You spend decades drawing reams of derivative manga and anime. The world comes to adore you as "
		"you churn out garbage day-in and day-out. Your net worth goes up and up and up.\n\nOne day, you look in the mirror of the upstairs bathroom in "
		"your third mansion. Egads! Not only was your career stupid, but you're almost bald!", { {"Combover", combover}, {"End It All", endItAll}}};
	StoryNode* pursueArt = new StoryNode{ 9, "Art!", "You try your hardest to come up with challenging, avant-garde cultural products. You spend years "
		"wowing a small circle of nerdy critics. But ultimately, nobody cares.\n\n\n\nEnough art, time to get that bling.", {{"Pursue Money", pursueMoney}}};

	StoryNode* winspiration = new StoryNode{ 8, "Inspiration", "Your epic victory over Sheila inspires you to become a writer. Whole vistas of plot-free "
		"print and animation expand before you!\n\nIt's not the most intelligent career, but it'll probably make money...",
		{{"Pursue Art", pursueArt}, {"Pursue Money", pursueMoney}} };

	StoryNode* useBeam = new StoryNode{ 7, "The Arc Concludes!", "Much to your own surprise, you shoot a beam out of your hand as well! The beams clash "
		"in midair, moving back and forth as the two of you struggle for dominance. You get your constipation-face on and give it your best beam-yelling. "
		"Slowly, the nexus of the beams moves towards Sheila until finally, she is utterly destroyed!", { {"Huzzah.", winspiration} }};
	StoryNode* deflect = new StoryNode{ 7, "Deflection!", "You deflect the beam with your cool samurai sword.\n\nThat was awesome, but your sword is "
		"broken now.", { {"Use Your Own Beam This Time", useBeam}} };
	StoryNode* takeHit = new StoryNode{ 7, "Vaporized!", "You take the beam like a champ. Your bravery will be remembered!"
		"\n\n[This is an ending. Quit to restart]" };

	StoryNode* telekinesis = new StoryNode{ 6, "Telekinesis!", "You use your latent telekinesis powers to fling a kitchen towel in Sheila's face!"
		"\nShe takes a few moments to recover, but then she somersaults into the air and her feet stick to the ceiling! You try to run away, but she "
		"fires a fast-moving laser beam out of her hand!", {{"Use Your Own Beam", useBeam}, {"Take The Hit", takeHit}, {"Deflect It", deflect}}};
	//DBZ/BUSINESS ARC

	StoryNode* shoveHer = new StoryNode{ 5, "The Shove", "You shove Sheila with all your might.\n\nShe doesn't move at all, and you go careening"
		" backwards.\nYou forgot that Sheila lifts and knows Taekwondo!\n\nYou tremble in fear as she charges and prepares to smack you in the face.",
		{{"Negotiate", negotiate}, {"Accept Your Fate", acceptFate}, {"Telekinesis", telekinesis}}};
	StoryNode* accuseHer = new StoryNode{ 5, "The Accusation", "You accuse Sheila of making this mess!\n\n\nShe just stares at you angrily.\n\nOh well.",
							{{"Shove Her", shoveHer}}};
	StoryNode* amnesia = new StoryNode{ 5, "Amnesia", "You pretend not to know who you are. Sheila says,\n\n\"You've already tried that 3 times"
		" this week!\"\n\nOh well. Guess it was worth a shot...", {{"Accuse Her", accuseHer}, {"Shove Her", shoveHer}} };

	StoryNode* goPee = new StoryNode{ 4, "Intercepted!", "As you rush to the bathroom, your roommate Sheila appears out of nowhere and"
		" blocks your way.\n\n\"I can't believe the state of this place! I left for 2 days and you've trashed everything. "
		"Clean it up now!\"\n\nThis is gonna be a serious time delay.", {{"Amnesia", amnesia}, {"Accuse Her", accuseHer}, {"Shove Her", shoveHer}}};

	StoryNode* goOutside = new StoryNode{ 3, "The Outside", "You walk out into the living room. It's not a pretty sight.\n\nThe trash is full of trash"
		" and so is the recycling. Loose papers have long ago flowed off your desk and onto the floor. In the far corner, a pyramid of takeout containers "
		"is on the brink of collapse.\n\nYou peek into the kitchen. The refrigrator door is tastefully cracked open. A tower of dishes rises high above"
		" the sink.\n\nSuddenly, you have an overwhelming urge to go pee.", {{"Go Pee Immediately", goPee}}};
	StoryNode* cleanChipsOff = new StoryNode{ 3, "Coming Clean", "You wipe the pieces of chip off your foot.\n\nNow you have foot smell on your hand,"
		" and you realize the pieces of chip just went back on the floor anyways.\n\nYou put your foot back down and "
		"step in the chip crumbs again.", {{"Go Outside", goOutside}} };
	StoryNode* tastyChips = new StoryNode{ 3, "Tasty Chips", "It's salty with a hint of foot.\n\nSeems you're a true connoisseur.", {{"Go Outside", goOutside}, {"Clean your foot", cleanChipsOff}} };

	StoryNode* wakeUp = new StoryNode{ 2, "The Awakening", "You get out of bed and immediately step on a bag of chips. Seems like it's going to be that kind of day.", {{"Go Outside", goOutside}, {"Clean your foot", cleanChipsOff}, {"Eat the chip", tastyChips}} };
	StoryNode* snooze = new StoryNode{ 2, "The Snooze", "You snooze the alarm. You continue to sleep...\n\n\nBut not for long.", {{"Zzzzz (Press 1)", nullptr}} };

	StoryNode* alarm = new StoryNode{ 1, "Alarm", "The alarm is going off again.\n\nIt's really loud.", { {"Get Up (Press 1)", wakeUp}, {"Snooze (Press 2)", snooze} } };
	snooze->options[0].targetNode = alarm;
	resistResponsibleness->options[0].targetNode = alarm;
	hospital->options[1].targetNode = alarm;

	root = new StoryNode{ 0, "The Beginning", "You wake up in your room.\n\nIt's already past 3 PM.", {{"Continue (Press 1)", alarm}} };
	breakUrWord->options[0].targetNode = root;
	giveInToResponsibleness->options[0].targetNode = root;
	praiseJesus->options[0].targetNode = root;
	mormonism->options[0].targetNode = root;
	endItAll->options[0].targetNode = root;

	activate_node(root);
}

PlayMode::~PlayMode() {
	for (const auto& fontMapPair : TextRenderer::renderedGlyphs)
	{
		for (const auto& keyValuePair : fontMapPair.second)
		{
			glDeleteTextures(1, &keyValuePair.second.textureID);
		}
	}

}

void free_story_nodes(StoryNode* node)
{
	for (OptionNode option : node->options)
	{
		if (option.targetNode != nullptr && option.targetNode->level > node->level)
			free_story_nodes(option.targetNode);
	}
	delete node;
}

void PlayMode::activate_node(StoryNode* node)
{
	if (node == visions)
		currentBodyRenderer = &psychBodyRenderer;
	currentNode = node;
	titleRenderer.set_text(node->title);
	currentBodyRenderer->set_text(node->body);
	std::string optionsString = "";
	for (int i = 0; i < node->options.size(); i++)
	{
		optionsString += node->options[i].label;
		if (i < node->options.size() - 1)
			optionsString += "                    ";
	}
	optionRenderer.set_text(optionsString);


}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) 
	{
		if (evt.key.keysym.sym == SDLK_1) 
		{
			if( currentNode->options.size() >= 1 && currentNode->options[0].targetNode != nullptr)
				activate_node(currentNode->options[0].targetNode);
			return true;
		}
		if (evt.key.keysym.sym == SDLK_2)
		{
			if (currentNode->options.size() >= 2 && currentNode->options[1].targetNode != nullptr)
				activate_node(currentNode->options[1].targetNode);
			return true;
		}
		if (evt.key.keysym.sym == SDLK_3)
		{
			if (currentNode->options.size() >= 3 && currentNode->options[2].targetNode != nullptr)
				activate_node(currentNode->options[2].targetNode);
			return true;
		}
	}
	return false;
}

void PlayMode::update(float elapsed) {

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {

	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	titleRenderer.draw();
	currentBodyRenderer->draw();
	optionRenderer.draw();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GL_ERRORS();
}	

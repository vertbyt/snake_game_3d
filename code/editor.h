
#ifndef EDITOR_H
#define EDITOR_H


namespace editor {

entities::Entity* get_entities();
void set_snake(entities::Snake_Color color, entities::Snake_Movement movement);

void init();
void update();
void render();

}

#endif


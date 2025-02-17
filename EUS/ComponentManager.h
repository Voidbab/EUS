#pragma once
#include "DrawableComponent.h"
#include "Require.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <list>

class ComponentManager {
private:
	std::list<Component* const> components;
	std::list<DrawableComponent* const> drawableComponents;

	std::list<Component* const> destroyedComponents;

	void freeDestroyedComponents();
public:
	ComponentManager();

	const bool containsComponent(const Component* const component) const;
	const bool removeComponent(Component* const component);
	const bool addComponent(Component* const component);

	void freeComponents();

	void update(const float deltaTime);
	void draw(const float deltaTime);

	template<class T> T* const getComponent() const {
		T* foundComponent = nullptr;

		std::find_if(components.begin(), components.end(), [=, &foundComponent](Component* component) {
			foundComponent = dynamic_cast<T*>(component);

			return foundComponent != nullptr;
		});

		return foundComponent;
	}

	Component* const getComponent(std::function<bool(Component const*)> predicate) {
		auto result = std::find_if(components.begin(), components.end(), predicate);

		return *result;
	}

	~ComponentManager();
};


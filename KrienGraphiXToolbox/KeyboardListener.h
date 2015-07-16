

#pragma once

#include <QKeyEvent>

namespace kgxt
{
	class KeyEvent
	{
		public:
			enum class KeyState
			{
				Pressed,
				Released
			};

			enum KeyModifiers
			{
				NoModifier = Qt::NoModifier,
				Shift      = Qt::ShiftModifier,
				Control    = Qt::ControlModifier,
				Alt        = Qt::AltModifier,
				Super      = Qt::MetaModifier,
				Keypad     = Qt::KeypadModifier
			};

			typedef Qt::Key Key;

			KeyEvent( const Key &key, const KeyState &keyState, const KeyModifiers &mod ) : m_key(key), m_keyState(keyState), m_modifiers(mod) {}

			Key keyCode() const { return m_key; }
			KeyState keyState() const { return m_keyState; }
			KeyModifiers modifiers() const { return m_modifiers; }

		private:
			Key m_key;
			KeyState m_keyState;
			KeyModifiers m_modifiers;
	};

	class KeyboardListener
	{
		public:
			virtual void keyPressed( const KeyEvent &evt )  = 0;
			virtual void keyReleased( const KeyEvent &evt ) = 0;

		protected:
			KeyboardListener() {}
			virtual ~KeyboardListener() {}

		private:
	};

}
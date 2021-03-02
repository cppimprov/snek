#pragma once

#include <functional>
#include <utility>

namespace snek
{

	namespace sdl
	{

		template<class T, class Deleter = std::function<void(T*)>>
		class object_handle
		{
		public:

			object_handle() = default;

			bool is_valid() const;
			void destroy();

			T* get_handle() const;

		protected:

			object_handle(T* object, Deleter deleter);

			void reset(T* object, Deleter deleter);

		private:

			std::unique_ptr<T, Deleter> m_handle;
		};

		template<class T, class D>
		object_handle<T, D>::object_handle(T* object, D deleter):
			m_handle(object, std::move(deleter)) { }
		
		template<class T, class D>
		bool object_handle<T, D>::is_valid() const
		{
			return (m_handle != nullptr);
		}

		template<class T, class D>
		T* object_handle<T, D>::get_handle() const
		{
			return m_handle.get();
		}

		template<class T, class D>
		void object_handle<T, D>::destroy()
		{
			m_handle.reset();
		}
		
		template<class T, class D>
		void object_handle<T, D>::reset(T* object, D deleter)
		{
			destroy();

			m_handle = { object, std::move(deleter) };
		}

	} // sdl

} // snek

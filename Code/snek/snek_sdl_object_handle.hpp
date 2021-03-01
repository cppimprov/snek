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

			object_handle();

			object_handle(object_handle const&) = delete;
			object_handle& operator=(object_handle const&) = delete;

			object_handle(object_handle&& other);
			object_handle& operator=(object_handle&& other);

			~object_handle();

			bool is_valid() const;
			void destroy();

			T* get_handle() const;

		protected:

			object_handle(T* object, Deleter deleter);

			void reset(T* object, Deleter deleter);

		private:

			T* m_handle;
			Deleter m_deleter;
		};

		template<class T, class D>
		object_handle<T, D>::object_handle():
			m_handle(nullptr) { }

		template<class T, class D>
		object_handle<T, D>::object_handle(T* object, D deleter):
			object_handle()
		{
			reset(object, std::move(deleter));
		}
		
		template<class T, class D>
		object_handle<T, D>::object_handle(object_handle&& other):
			object_handle()
		{
			reset(other.m_handle, std::move(other.m_deleter));
			other.destroy();
		}

		template<class T, class D>
		object_handle<T, D>& object_handle<T, D>::operator=(object_handle&& other)
		{
			auto temp = std::move(other);

			using std::swap;
			swap(m_handle, temp.m_handle);
			swap(m_deleter, temp.m_deleter);

			return *this;
		}

		template<class T, class D>
		object_handle<T, D>::~object_handle()
		{
			destroy();
		}

		template<class T, class D>
		bool object_handle<T, D>::is_valid() const
		{
			return (m_handle != nullptr);
		}

		template<class T, class D>
		T* object_handle<T, D>::get_handle() const
		{
			return m_handle;
		}

		template<class T, class D>
		void object_handle<T, D>::destroy()
		{
			if (m_deleter)
				m_deleter(m_handle);

			m_handle = nullptr;
			m_deleter = D();
		}
		
		template<class T, class D>
		void object_handle<T, D>::reset(T* object, D deleter)
		{
			destroy();

			m_handle = object;
			m_deleter = std::move(deleter);
		}

	} // sdl

} // snek

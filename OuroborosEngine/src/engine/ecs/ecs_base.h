#pragma once

#include <bitset>
#include <type_traits>
#include <vector>

#include "../common.h"
#include "../custom_assert.h"
#include "brigand/algorithms.hpp"
#include "brigand/brigand.hpp"
#include "brigand/sequences.hpp"
#include "brigand/adapted/tuple.hpp"
#include "ecs/component_manager.h"
#include "ecs/system_manager.h"

namespace OE
{
	namespace impl
	{
		template<typename T>
		struct _as_vector
		{
			using type = std::vector<T>;
		};

		template <typename T, typename Tuple>
		struct _tuple_has_type;

		template <typename T>
		struct _tuple_has_type<T, std::tuple<>> : std::false_type {};

		template <typename T, typename U, typename... Ts>
		struct _tuple_has_type<T, std::tuple<U, Ts...>> : _tuple_has_type<T, std::tuple<Ts...>> {};

		template <typename T, typename... Ts>
		struct _tuple_has_type<T, std::tuple<T, Ts...>> : std::true_type {};

		template <typename T, typename List>
		struct _list_has_type;

		template <typename T>
		struct _list_has_type<T, brigand::list<>> : std::false_type {};

		template <typename T, typename U, typename... Ts>
		struct _list_has_type<T, brigand::list<U, Ts...>> : _list_has_type<T, brigand::list<Ts...>> {};

		template <typename T, typename... Ts>
		struct _list_has_type<T, brigand::list<T, Ts...>> : std::true_type {};
	}
	namespace ECS
	{
		template<typename ...TS>
		using TypeList = brigand::list<TS...>;

		namespace types
		{
			template<typename... TS>
			using Signature = TypeList<TS...>;
			template<typename... TS>
			using SignatureList = TypeList<TS...>;
			template<typename... TS>
			using ComponentList = TypeList<TS...>;
			template<typename... TS>
			using SystemList = TypeList<TS...>;
		}

		class ID_Generator
		{
		public:
			static ecs_ID GenerateID()
			{
				//OE_Assert(counter == INT16_MAX, "ID encountered maximum size of ID generator");
				return counter++;
			}
			static void Reset()
			{
				counter = 0;
			}
			inline static ecs_ID counter{ 0 };
		};

		namespace _types
		{
			template<typename TSetting>
			struct Entity
			{
				using Bitset = typename TSetting::bitset;
				ecs_ID myID{};
				bool alive = false;
				Bitset bitset{};
			};
		}

		namespace _impl
		{
			template<class TSettings>
			class SignatureBitsetStorage;

			template<typename TComponentList, typename TSignatureList, typename TSystemList>
			struct Settings
			{
				using type = Settings<TComponentList, TSignatureList, TSystemList>;

				using component_list = TComponentList;
				using signature_list = TSignatureList;
				using system_list    = TSystemList;
				using signature_bitset_storage = SignatureBitsetStorage<type>;
				using system_storage = SystemManager<TSystemList>;
				
				template<typename T>
				static constexpr bool IsComponent() noexcept
				{
					return impl::_list_has_type<T, component_list>::value;
				}

				template<typename T>
				static constexpr bool IsSignature() noexcept
				{
					return impl::_list_has_type<T, signature_list>::value;
				}

				struct _impl
				{
					template<typename T>
					struct _IsSignature : impl::_list_has_type<T, signature_list> {};

					template<typename T>
					struct _IsSystem : impl::_list_has_type<T, system_list> {};
				};

				template<typename T>
				static constexpr bool IsSystem() noexcept
				{
					return impl::_list_has_type<T, system_list>::value;
				}

				static constexpr size_t CountComponent() noexcept
				{
					return brigand::size<component_list>::value;
				}

				static constexpr size_t CountSignature() noexcept
				{
					return brigand::size<signature_list>::value;
				}

				static constexpr size_t CountSystem() noexcept
				{
					return brigand::size<system_list>::value;
				}

				template<typename T>
				static constexpr size_t ComponentIndex() noexcept
				{
					return brigand::index_of<component_list, T>::value;
				}

				template<typename T>
				static constexpr size_t SignatureIndex() noexcept
				{
					static_assert(IsSignature<T>());
					return brigand::index_of<signature_list, T>::value;
				}

				template<typename T>
				static constexpr size_t SystemIndex() noexcept
				{
					static_assert(IsSystem<T>());
					return brigand::index_of<system_list, T>::value;
				}

				template<typename T>
				static constexpr size_t ComponentBit() noexcept
				{
					static_assert(IsComponent<T>());
					return ComponentIndex<T>();
				}

				template<typename T>
				static constexpr size_t SystemBit() noexcept
				{
					static_assert(IsSystem<T>());
					return CountComponent() + SystemIndex<T>();
				}

				using bitset = std::bitset<CountSystem() + CountComponent()>;

				template<typename T>
				struct GetSignature
				{
					static_assert(IsSignature<T>());
					using type = brigand::at<signature_list, std::integral_constant<size_t, SignatureIndex<T>()>>;
				};

				template<typename T>
				struct GetSystem
				{
					static_assert(IsSystem<T>());
					using type = brigand::at<system_list, std::integral_constant<size_t, SystemIndex<T>()>>;
				};

				using system_status = std::array<OE::Status, CountSystem() + CountSignature()>;
			};

			template<typename TSettings>
			class SignatureBitsetStorage
			{
			public:
				using settings = TSettings;
				using type = SignatureBitsetStorage<settings>;
				using component_list = typename settings::component_list;
				using signature_list = typename settings::signature_list;
				using system_list    = typename settings::system_list;
				using tuple_component_list = brigand::as_tuple<typename settings::component_list>;
				using tuple_signature_list = brigand::as_tuple<typename settings::signature_list>;
				using tuple_system_list    = brigand::as_tuple<typename settings::system_list>;
				using bitset = typename settings::bitset;

				using bitset_count = std::integral_constant<size_t, settings::CountSignature() + settings::CountSystem()>;
				using bitset_storage_type = brigand::as_tuple < brigand::filled_list < bitset, bitset_count{} >>;

				//template<typename T>
				//using IsComponentFilter = std::bool_constant < settings::template IsComponent<T>() >;
				template<typename T>
				struct IsComponentFilter : std::bool_constant<settings::template IsComponent<T>()>{};
				template<typename T>
				struct IsSystemFilter : std::bool_constant < settings::template IsSystem<T>()> {};

				using settings = TSettings;

				bitset_storage_type bitset_storage;

				template<typename TSignature>
				using SignatureComponents = brigand::remove_if<TSignature, IsSystemFilter<brigand::_1>>;
				template<typename TSystem>
				using SignatureSystems = brigand::remove_if<TSystem, IsComponentFilter<brigand::_1>>;

			public:
				template<typename T, std::enable_if_t<settings::_impl::template _IsSignature<T>::value, T>* = nullptr>
				auto& GetSignatureBitset() noexcept
				{
					//static_assert(settings::template IsSignature<T>());
					constexpr size_t index = settings::template SignatureIndex<T>();
					return std::get<index>(bitset_storage);
				}

				template<typename T, std::enable_if_t<settings::_impl::template _IsSystem<T>::value, T>* = nullptr>
				auto& GetSignatureBitset() noexcept
				{
					//static_assert(settings::template IsSignature<T>());
					constexpr size_t index = settings::CountSignature() + settings::template SystemIndex<T>();
					return std::get<index>(bitset_storage);
				}

				template<typename T, std::enable_if_t<settings::_impl::template _IsSignature<T>::value, T>* = nullptr>
				const auto& GetSignatureBitset() const noexcept
				{
					//static_assert(settings::template IsSignature<T>());
					constexpr size_t index = settings::template SignatureIndex<T>();
					return std::get<index>(bitset_storage);
				}

				template<typename T, std::enable_if_t<settings::_impl::template _IsSystem<T>::value, T>* = nullptr>
				const auto& GetSignatureBitset() const noexcept
				{
					//static_assert(settings::template IsSignature<T>());
					constexpr size_t index = settings::CountSignature() + settings::template SystemIndex<T>();
					return std::get<index>(bitset_storage);
				}

				constexpr size_t GetSystemsCount() const noexcept
				{
					return brigand::sizeof_<system_list>();
				}

			private:
				template<typename TSignature,
					std::enable_if_t<settings::_impl::template _IsSignature<TSignature>::value, TSignature>* = nullptr>
				void InitializeBitset() noexcept
				{
					auto& bitset = GetSignatureBitset<TSignature>();
					using Components = SignatureComponents<TSignature>;
					using Systems	 = SignatureSystems<TSignature>;
					
					brigand::for_each<Components>([this, &bitset](auto s)
						{
							using TComponent = typename decltype(s)::type;
							bitset[settings:: template ComponentBit<TComponent>()] = true;
						});

					brigand::for_each<Systems>([this, &bitset](auto s)
						{
							using TSystem = typename decltype(s)::type;
							bitset[settings:: template SystemBit<TSystem>()] = true;
						});
				}

				template<typename TSystem, std::enable_if_t<settings::_impl::template _IsSystem<TSystem>::value, TSystem>* = nullptr>
				void InitializeBitset() noexcept
				{
					auto& bitset = GetSignatureBitset<TSystem>();
					using Components = SignatureComponents<TSystem>;
					using Systems = brigand::list<TSystem>/*SignatureSystems<TSystem>*/;
					
					brigand::for_each<Components>([this, &bitset](auto s)
						{
							using TComponent = typename decltype(s)::type;
							bitset[settings:: template ComponentBit<TComponent>()] = true;
						});

					brigand::for_each<Systems>([this, &bitset](auto s)
						{
							using system = typename decltype(s)::type;
							bitset[settings:: template SystemBit<system>()] = true;
						});
					
				}
			public:
				SignatureBitsetStorage() noexcept
				{
					brigand::for_each<signature_list>([this](auto t)
						{
							using signature = typename decltype(t)::type;
							InitializeBitset<signature>();
						});
					brigand::for_each<system_list>([this](auto t)
						{
							using system = typename decltype(t)::type;
							InitializeBitset<system>();
						});
				}
			};
		}

		template<class TSettings>
		class Manager
		{
		public:
			using settings = TSettings;
		private:
			using type = Manager<settings>;
			using SignatureBitsetStorage = typename settings::signature_bitset_storage;
			//using SystemStorage = typename settings::system_storage;
			using Entity = _types::Entity<settings>;

			using EntityStorage = std::vector<Entity>;
			using SystemStatusStorage = std::vector<typename settings::system_status>;
		public:
			using SystemStorage = typename settings::system_storage;
			using ComponentStorage = typename SignatureBitsetStorage::component_list;

			EntityStorage				entity_storage;
			ComponentManager<TSettings> component_manager;
			SignatureBitsetStorage		signature_bitset_storage{};
			SystemStorage				system_storage{};
			SystemStatusStorage		    system_status;

			ecs_ID						num_entities{ 0 };
			size_t						current_container_size{ 0 };

			void GrowCapacity(int64_t new_capacity) noexcept
			{
				current_container_size = new_capacity;
				component_manager.GrowCapacity(current_container_size);
				entity_storage.resize(current_container_size);
				system_status.resize(current_container_size);
			}

			[[nodiscard]] bool NeedGrowCapacity() const
			{
				return num_entities + 10 >= current_container_size;
			}

			void GrowthIfNeeded()
			{
				if (!NeedGrowCapacity())
				{
					return;
				}
				GrowCapacity((current_container_size + 10) * 2);
			}

			template<typename T>
			constexpr void AssertComponentType()
			{
				static_assert(settings::template IsComponent<T>());
			}
		public:
			void Clear()
			{
				for (ecs_ID idx = 0; idx < num_entities; ++idx)
				{
					DeleteEntity(idx);
				}
				current_container_size = 0;
				num_entities = 0;
				component_manager.GrowCapacity(current_container_size);
				entity_storage.resize(current_container_size);
				system_status.resize(current_container_size);
				ID_Generator::Reset();
			}

			Entity& CreateEntity()
			{
				GrowthIfNeeded();
				auto created_entityID = ID_Generator::GenerateID();
				Entity entity_created;
				entity_created.alive = true;
				entity_created.myID = created_entityID;
				++num_entities;
				entity_storage[created_entityID] = entity_created;
				return GetEntity(created_entityID);
				//return entity_storage[num_entities++];
			}

			Entity& GetEntity(ecs_ID entityID)
			{
				return entity_storage[entityID];
			}

			const Entity& GetEntity(ecs_ID entityID) const
			{
				return entity_storage[entityID];
			}

			template<typename T>
			bool HasComponent(ecs_ID entityID)
			{
				AssertComponentType<T>();
				return entity_storage[entityID].bitset[settings::template ComponentBit<T>()] == true;
			}

			template<typename T, typename ...TArgs>
			auto& AddComponent(ecs_ID entityID, TArgs&& ...args)
			{
				AssertComponentType<T>();
				OE_Assert(!HasComponent<T>(entityID), "");

				Entity& entity = GetEntity(entityID);
				entity.bitset[settings::template ComponentBit<T>()] = true;

				T& component = component_manager.template GetComponent<T>(entityID);
				new (&component) T(std::move(args)...);

				return component;
			}

			template<typename T>
			void DeleteComponent(ecs_ID entityID)
			{
				AssertComponentType<T>();
				OE_Assert(HasComponent<T>(entityID), "");

				Entity& entity = GetEntity(entityID);
				entity.bitset[settings::template ComponentBit<T>()] = false;

				component_manager.template GetComponent<T>(entityID);
			}

			template<typename TSystem>
			bool AddSystem(ecs_ID entityID)
			{
				//static_assert(SignatureBitsetStorage::template IsSystem<TSystem>(), "");
				//OE_Assert(!HasComponent<TSystem>(entityID), "");

				Entity& entity = GetEntity(entityID);
				Status& current_status = GetFunctionStatus<TSystem>(entity.myID);
				current_status = Status::CREATED;

				if (entity.bitset[settings::template SystemBit<TSystem>()] == true)
				{
					return false;
				}

				//entity.bitset[settings::template SystemBit<TSystem>()] = true;

				return true;
			}

			template<typename TSystem>
			bool DeleteSystem(ecs_ID entityID)
			{
				//static_assert(SignatureBitsetStorage::template IsSystem<TSystem>(), "");
				//OE_Assert(HasComponent<TSystem>(entityID), "");
				Entity& entity = GetEntity(entityID);

				Status& current_status = GetFunctionStatus<TSystem>(entity.myID);
				current_status = Status::DESTROYED;

				if (entity.bitset[settings::template SystemBit<TSystem>()] == false)
				{
					return false;
				}

				//entity.bitset[settings::template SystemBit<TSystem>()] = false;

				return true;
			}
			template<typename TSignature, std::enable_if_t<settings::_impl::template _IsSignature<TSignature>::value, TSignature>* = nullptr>
			Status& GetFunctionStatus(ecs_ID entityID)
			{
				return system_status[entityID][settings::template SignatureIndex<TSignature>()];
			}

			template<typename TSystem, std::enable_if_t<settings::_impl::template _IsSystem<TSystem>::value, TSystem>* = nullptr>
			Status& GetFunctionStatus(ecs_ID entityID)
			{
				return system_status[entityID][settings::template CountSignature() + settings::template SystemIndex<TSystem>()];
			}

			template<typename TSystem>
			bool HasSystem(ecs_ID entityID)
			{
				//static_assert(!SignatureBitsetStorage::template IsSystem<TSystem>(), "");

				Entity& entity = GetEntity(entityID);
				auto systemIndex = settings::template SystemBit<TSystem>();
				return entity.bitset[settings::template SystemBit<TSystem>()];
			}

			void DeleteEntity(ecs_ID entityID)
			{
				Entity& entity = GetEntity(entityID);
				entity.bitset.reset();
				entity.alive = false;
			}

			template<typename T>
			auto& GetComponent(ecs_ID entityID) noexcept
			{
				OE_Assert(HasComponent<T>(entityID), "");
				return component_manager.template GetComponent<T>(entityID);
			}

			void Refresh() noexcept
			{
				//ecs_ID index_alive{ 0 }, index_dead{ current_container_size - 1 };
				//while(true)
				//{
				//	
				//}
			}

			size_t NumEntitiesAlive() const noexcept
			{
				return num_entities;
			}

			template<typename T>
			bool MatchesSignature(ecs_ID entityID) const noexcept
			{
				//static_assert(settings::template IsSignature<T>());
				using Bitset = typename settings::bitset;
					  
				const auto& entity_bitset    = GetEntity(entityID).bitset;
				const auto& signature_bitset = signature_bitset_storage.template GetSignatureBitset<T>();
				
				return (entity_bitset & signature_bitset) == signature_bitset;
			}

			template<typename TFunction>
			void ForEntities(TFunction&& function)
			{
				for(ecs_ID idx = 0; idx < num_entities; ++idx)
				{
					function(idx);
				}
			}

			template<typename TSystem, std::enable_if_t<settings::_impl::template _IsSystem<TSystem>::value, TSystem>* = nullptr>
			void UpdateStatus(ecs_ID entity_ID)
			{
				bool match_signature = MatchesSignature<TSystem>(entity_ID);
				switch (Status& current_status = GetFunctionStatus<TSystem>(entity_ID))
				{
				case Status::CREATED:
					GetEntity(entity_ID).bitset[settings::template SystemBit<TSystem>()] = true;
					current_status = Status::INIT;
					break;
				case Status::INIT:
					//GetEntity(entity_ID).bitset[settings::template SystemBit<TSystem>()] = true;
					current_status = Status::UPDATE;
					break;
				case Status::UPDATE:
					break;
				case Status::DESTROYED:
					current_status = Status::CLEANUP;
					break;
				case Status::CLEANUP:
					GetEntity(entity_ID).bitset[settings::template SystemBit<TSystem>()] = false;
					break;

				default:
					break;
				}
			}

			template<typename TSignature, typename TF, std::enable_if_t<settings::_impl::template _IsSignature<TSignature>::value, TSignature>* = nullptr>
			void ForEntitiesMatching(float dt, TF&& function) noexcept
			{
				ForEntities([this, &dt, &function](ecs_ID i)
					{
						if (MatchesSignature<TSignature>(i))
						{
							ExpandSignatureCall<TSignature>(GetFunctionStatus<TSignature>(i), i, dt, function);
						}
					});
			}

			template<typename TSystem, typename TF, std::enable_if_t<settings::_impl::template _IsSystem<TSystem>::value, TSystem>* = nullptr>
			void ForEntitiesMatching(float dt, TF&& function) noexcept
			{
				using signature = typename TSystem::signature;
				using required_components = typename SignatureBitsetStorage::template SignatureComponents<signature>;
				ForEntities([this, &dt, &function](ecs_ID i)
					{
						UpdateStatus<TSystem>(i);
						if (MatchesSignature<TSystem>(i))
						{
							ExpandSignatureCall<required_components>(GetFunctionStatus<TSystem>(i), i, dt, function);
						}
					});
			}

			//template<typename TSystem, typename Obj, typename TF, std::enable_if_t<settings::_impl::template _IsSystem<TSystem>::value, TSystem>* = nullptr>
			//void ForEntitiesMatching(float dt, Obj* obj, TF&& function) noexcept
			//{
			//	//static_assert(std::false_type());
			//	using signature = typename TSystem::signature;
			//	using required_components = typename SignatureBitsetStorage::template SignatureComponents<signature>;
			//	ForEntities([this, &dt, &function, &obj](ecs_ID i)
			//		{
			//			if (MatchesSignature<TSystem>(i))
			//			{
			//				ExpandSignatureCall<required_components, Obj>(GetFunctionStatus<TSystem>(i), obj, i, dt, function);
			//			}
			//		});
			//}

			/*template<typename TSignature, typename TF, std::enable_if_t<settings::_impl::template _IsSignature<TSignature>::value, TSignature>* = nullptr>
			void ForEntitiesMatching(float dt, TF& function) noexcept
			{
				ForEntities([this, &dt, &function](ecs_ID i)
					{
						if (MatchesSignature<TSignature>(i))
						{
							ExpandSignatureCall<TSignature>(GetFunctionStatus<TSignature>(i), i, dt, function);
						}
					});
			}

			template<typename TSystem, typename TF, std::enable_if_t<settings::_impl::template _IsSystem<TSystem>::value, TSystem>* = nullptr>
			void ForEntitiesMatching(float dt, TF& function) noexcept
			{
				using signature = typename TSystem::signature;
				using required_components = typename SignatureBitsetStorage::template SignatureComponents<signature>;
				ForEntities([this, &dt, &function](ecs_ID i)
					{
						if (MatchesSignature<TSystem>(i))
						{
							ExpandSignatureCall<required_components>(GetFunctionStatus<TSystem>(i), i, dt, function);
						}
					});*/
			//}

			template<typename TSystem>
			void UpdateNativeSystem(float dt)
			{
				using system_list = typename settings::system_list;
				//brigand::for_each<system_list>([&](auto type)
				//	{
						using system = TSystem;
						if (auto& system_implementation = system_storage.template GetSystemImplementation<system>())
						{
							this->template ForEntitiesMatching<system>(dt, system_implementation);
						}
					//});
			}
		private:
			template<typename ...Ts>
			struct ExpandCallHelper;

			template <typename... T>
			using expand_call_wrapper = ExpandCallHelper<T...>;

			template <typename L>
			using as_expand_call = brigand::wrap<L, expand_call_wrapper>;

			//template<typename T, typename Obj, typename TF>
			//void ExpandSignatureCall(Status status, Obj* obj, ecs_ID mI, float dt, TF&& mFunction)
			//{
			//	//static_assert(settings::template IsSignature<T>(), "");
			//	using Components = typename SignatureBitsetStorage::template SignatureComponents<T>;
			//	using Helper = as_expand_call<Components>;
			//	Helper::Call(status, obj, mI, dt, *this, mFunction);
			//}

			template<typename T, typename TF>
			void ExpandSignatureCall(Status status, ecs_ID mI, float dt, TF&& mFunction)
			{
				static_assert(settings::template IsSignature<T>(), "");
				using Components = typename SignatureBitsetStorage::template SignatureComponents<T>;
				using Helper = as_expand_call<Components>;
				Helper::Call(status, mI, dt, *this, mFunction);
			}

			template<typename T, typename TF>
			void ExpandSignatureCall(Status status, ecs_ID mI, float dt, TF& mFunction)
			{
				//static_assert(settings::template IsSignature<T>(), "");
				using Components = typename SignatureBitsetStorage::template SignatureComponents<T>;
				using Helper = as_expand_call<Components>;
				Helper::Call(status, mI, dt, *this, mFunction);
			}

			template<typename ...Ts>
			struct ExpandCallHelper
			{
				template<typename TF>
				static void Call(Status status, ecs_ID mI, float dt, type& mMgr, TF&& mFunction)
				{
					auto di(mMgr.GetEntity(mI).myID);

					mFunction
					(
						status,
						mI, dt,
						mMgr.component_manager.template GetComponent<Ts>(di)...
					);
				}

			//	template<typename TF, typename Obj>
			//	static void Call(Status status, Obj* obj, ecs_ID mI, float dt, type& mMgr, TF&& mFunction)
			//	{
			//		auto di(mMgr.GetEntity(mI).myID);

			//		mFunction(
			//			status,
			//			di,
			//			std::forward<ecs_ID>(mI), 
			//			std::forward<float>(dt), 
			//			mMgr.component_manager.template GetComponent<Ts>(di)...);

			//		//mFunction
			//		//(
			//		//	mI, dt,
			//		//	mMgr.component_manager.template GetComponent<Ts>(di)...
			//		//);
			//	}
			};
		};

	}
}

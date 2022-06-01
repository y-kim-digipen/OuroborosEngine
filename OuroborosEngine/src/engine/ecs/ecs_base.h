#pragma once

#include <vector>
#include <bitset>

#include "../common.h"
#include "../custom_assert.h"
#include "engine/ecs/component_manager.h"
#include "brigand/sequences.hpp"
#include "brigand/algorithms.hpp"
#include "brigand/adapted/tuple.hpp"

#include <type_traits>

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


		}
		class ID_Generator
		{
		public:
			static ecs_ID GenerateID()
			{
				//OE_Assert(counter == INT16_MAX, "ID encountered maximum size of ID generator");
				return counter++;
			}
			inline static ecs_ID counter{ 0 };
		};

		namespace _types
		{


			//template<typename TComponentList, typename TSignatureList>
			//struct Settings;

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

			template<typename TComponentList, typename TSignatureList>
			struct Settings
			{
				using type = Settings<TComponentList, TSignatureList>;

				using component_list = TComponentList;
				using signature_list = TSignatureList;
				using signature_bitset_storage = SignatureBitsetStorage<type>;


				template<typename T>
				static constexpr bool IsComponent() noexcept
				{
					return impl::_tuple_has_type<T, brigand::as_tuple<component_list>>::value;
				}

				template<typename T>
				static constexpr bool IsSignature() noexcept
				{
					return impl::_tuple_has_type<T, brigand::as_tuple<signature_list>>::value;
				}

				static constexpr size_t CountComponent() noexcept
				{
					return brigand::size<component_list>::value;
				}

				static constexpr size_t CountSignature() noexcept
				{
					return brigand::size<signature_list>::value;
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

				using bitset = std::bitset<CountComponent()>;

				template<typename T>
				struct GetSignature
				{
					static_assert(IsSignature<T>());
					using type = brigand::at<signature_list, std::integral_constant<size_t, SignatureIndex<T>()>>;
				};
			};


			template<typename TSettings>
			class SignatureBitsetStorage
			{
			public:
				using settings = TSettings;
				using type = SignatureBitsetStorage<settings>;
				using component_list = typename settings::component_list;
				using signature_list = typename settings::signature_list;
				using tuple_component_list = brigand::as_tuple<typename settings::component_list>;
				using tuple_signature_list = brigand::as_tuple<typename settings::signature_list>;
				using bitset = typename settings::bitset;

				using bitset_storage_type = brigand::as_tuple<brigand::filled_list<bitset, settings::CountSignature()>>;

				template<typename T>
				using IsComponentFilter = std::integral_constant<bool, settings::template IsComponent<T>()>;

				using settings = TSettings;

				bitset_storage_type bitset_storage;

			public:
				template<typename T>
				auto& GetSignatureBitset() noexcept
				{
					static_assert(settings::template IsSignature<T>());
					constexpr size_t index = settings::template SignatureIndex<T>();
					return std::get<index>(bitset_storage);
				}

				template<typename T>
				const auto& GetSignatureBitset() const noexcept
				{
					static_assert(settings::template IsSignature<T>());
					constexpr size_t index = settings::template SignatureIndex<T>();
					return std::get<index>(bitset_storage);
				}

			private:
				template<typename T>
				void InitializeBitset() noexcept
				{
					auto& bitset = GetSignatureBitset<T>();

					using Tsignature = typename settings:: template GetSignature<T>::type;
					brigand::for_each<Tsignature>([this, &bitset](auto s)
						{
							using Tcompomnent = decltype(s);
#ifdef _DEBUG
							std::cout << typeid(Tsignature).name() << typeid(Tcompomnent).name() << std::endl;
#endif
							bitset[settings:: template ComponentIndex<Tcompomnent::type>()] = true;
						});
				}

			public:
				SignatureBitsetStorage() noexcept
				{
					brigand::for_each<signature_list>([this](auto t)
						{
							InitializeBitset<typename decltype(t)::type>();
						});
				}
			};
		}

		template<class TSettings>
		class Manager
		{
			using settings = TSettings;
			using type = Manager<settings>;
			using SignatureBitsetStorage = typename settings::signature_bitset_storage;
			using Entity = _types::Entity<settings>;

			using EntityStorage = std::vector<Entity>;


		public:
			EntityStorage				entity_storage;
			ComponentManager<TSettings> component_manager;
			SignatureBitsetStorage		signature_bitset_storage{};

			ecs_ID						num_entities{ 0 };
			size_t						current_container_size{ 0 };

			void GrowCapacity(int64_t new_capacity) noexcept
			{
				current_container_size = new_capacity;
				component_manager.GrowCapacity(current_container_size);
				entity_storage.resize(current_container_size);
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
				return entity_storage[entityID].bitset[settings::template ComponentIndex<T>()] == true;
			}

			template<typename T, typename ...TArgs>
			auto& AddComponent(ecs_ID entityID, TArgs&& ...args)
			{
				AssertComponentType<T>();
				OE_Assert(!HasComponent<T>(entityID), "");

				Entity& entity = GetEntity(entityID);
				entity.bitset[settings::template ComponentIndex<T>()] = true;

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
				entity.bitset[settings::template ComponentIndex<T>()] = false;

				component_manager.template GetComponent<T>(entityID);
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
				static_assert(settings::template IsSignature<T>());
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

			template<typename Signature, typename TF>
			void ForEntitiesMatching(float ft, TF&& function)
			{
				ForEntities([this, &ft, &function](ecs_ID i)
					{
						if (MatchesSignature<Signature>(i))
						{
							ExpandSignatureCall<Signature>(i, ft, function);
						}
					});
			}

		private:
			template<typename ...Ts>
			struct ExpandCallHelper;

			template <typename... T>
			using expand_call_wrapper = ExpandCallHelper<T...>;

			template <typename L>
			using as_expand_call = brigand::wrap<L, expand_call_wrapper>;

			template<typename T, typename TF>
			void ExpandSignatureCall(ecs_ID mI, float dt, TF&& mFunction)
			{
				static_assert(settings::template IsSignature<T>(), "");
				using Helper = as_expand_call<T>;
				Helper::Call(mI, dt, *this, mFunction);
			}

			template<typename ...Ts>
			struct ExpandCallHelper
			{
				template<typename TF>
				static void Call(ecs_ID mI, float dt, type& mMgr, TF&& mFunction)
				{
					auto di(mMgr.GetEntity(mI).myID);
					mFunction
					(
						mI, dt,
						mMgr.component_manager.template GetComponent<Ts>(di)...
					);
				}
			};
		};
	}
}
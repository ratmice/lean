prelude
import init.meta.name init.meta.expr
meta structure local_decl :=
(unique_name : name)
(pp_name : name)
(type : expr)
(value : option expr)
(bi : binder_info)
(idx : nat)

/-- Local context. -/
meta constant lctxt : Type

namespace lctxt
  /-- Add a new local constant to the lctxt. The new local has an unused unique_name. Fails when the type depends on local constants that are not present in the context.-/
  meta constant mk_local_decl (pretty_name : name) (type : expr) (bi : binder_info) : lctxt → option (expr × lctxt)
  -- meta constant mk_local_decl_assigned (pretty_name : name) (type : expr) (value : expr) : lc expr
  meta constant get_local_decl : name → lctxt → option local_decl
  meta constant get_local : name → lctxt → option expr
  -- /-- Removes the local decl with the given unique name. Will fail if other decls in the context depend on it. -/
  -- meta constant clear : name → lctxt → lctxt 
  -- /-- Removes the local with the given unique name and recursively clears decls that depend on it. -/
  -- meta constant clear_recursive : name → lctxt → lctxt
  meta constant is_subset : lctxt → lctxt → bool
  meta constant to_local_list : lctxt → list expr
end lctxt


-- meta def hypothetically {α} (tac : tactic α) : tactic α :=
-- λ s, match tac s with
-- |(result.success a _) := result.success a s
-- |(result.exception ms pos _) := result.exception ms pos s
-- end

-- meta def my_tac : tactic unit := do
--   [g₁] ← get_goals,
--   (l,g₂) ← hypothetically (do
--     l ← intro `hello,
--     [g₂] ← get_goals,
--     pure (l, g₂)
--   ),
--   intro `hello2,
--   exact l

-- example : nat → nat := begin my_tac,  end


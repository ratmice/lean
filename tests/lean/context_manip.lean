

/- [TODO]
- `expr.instantiate_nth_var`

[TODO] rename this fine.
 -/

-- should be 0
#eval expr.get_free_var_range `(Type)
-- should be 2
#eval expr.get_free_var_range (expr.lam `hello binder_info.default `(Type) $ expr.app (expr.var 1) (expr.var 2))
-- should be 3
#eval expr.get_free_var_range (expr.app (expr.var 1) (expr.var 2))


open tactic
/-- Implementing `tactic.intro` within Lean using builtins for manipulating mvar context. -/
meta def my_intro : name → tactic expr | hn := do
  g::rest ← tactic.get_goals,
  type ← infer_type g,
  match type with
  |(expr.pi _ bi y b) := do
    lctx ← get_meta_var_context g,
    (H,lctx) ← lctxt.mk_local_decl hn y bi lctx,
    new_type ← pure $ expr.instantiate_var b H,
    new_M ← mk_meta_var_in new_type lctx,
    new_val ← pure $  expr.lam hn bi y $ expr.mk_delayed_abstraction new_M [H.local_uniq_name],
    set_goals $ new_M :: rest,
    assign_meta_var g new_val,
    pure H
  |_ := failure
  end

example : ∀ (x : ℕ), x = x := begin 
  my_intro `hello,
  refl
end
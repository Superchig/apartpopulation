# When the head of a family dies,
- If they have no eldest living child,
  - If they have any living family members, then their eldest family member inherits
  - If they have no remaining family members, then the family is destroyed
- If their eldest living child is an adult, then the family splits according
  to its adults, with the under-age children going with the eldest living
  child
- If their eldest living child is not an adult, then
  - If they have a spouse, the new head of the family is that spouse
  - If they do not have a spouse, the new head of the family is the eldest
    living child, even if they are not an adult

## Notes
- One of the above outcomes should happen whenever the head of a family dies

## References to a FamilyNode
- Each of its orbiters has it as their leader
- The head has a reference to it as its family
- If its leader is nullptr, then its plot will have a reference to it as a root family

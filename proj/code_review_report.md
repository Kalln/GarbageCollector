# Code Review Report

**Branches and pull requests**

During this project, in order to maintain a high integrity in the main code, we made sure to always have our work-in-progress in a separated branch.   
How these branches were used varied from person to person as some prefered to make a separate branch for each separate thing they were working on while others prefered to have a single branch.   
The important part here is that by working in branches we made sure that the main code wasn't compromised by any code that was faulty.   
When someone felt that their code was done and working as intended they could then do a pull request, asking to merge their code into the main code and it was then up to the others to review this code and decide if it should be merged or if changes had to be made.   

**Review process**

After creating a pull request, we decided that the code (in most cases) should be reviewed by two others before being merged.   
While this slows down the process a bit, it helps with finding errors that one reviewer might miss which in turn keeps the integrity of the main code.   
When reviewing someone else's code we looked for everything ranging from minor structural mistakes to big oversights.
If the code had some mistakes we would first request changes and only when two reviewers approve the code it would be merged into the main code.   

**Thoughts about this method**   
We believe that this slow but rigorous method of reviewing the code has led to less setbacks and in general a smoother process in this project.   
It wasn't perfect however as sometimes it would take several days for two code review to be done and if one of them requested changes it could take a very long time for some code to finally be implemented in the main code.   
If we were to change something it would probably be to do code reviews more frequently so that we could make faster progress.   
Finally regarding the colourful diffs when reviewing code in Github, it's a mix of good and bad.   
It can make it very clear what has been added/removed/changed in the code but when you review a larger messier pull request it can quickly become confusing and difficult to follow.   

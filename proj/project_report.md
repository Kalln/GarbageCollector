# Project Report

## Church

Group name: Church


| Name | Email | Active Dates |
|-------------------------|---|---|
| Alicia Sjöstrand | alicia.sjostrand.9519@student.uu.se | 12/04 - 01/09 |
| Emil Edin | emil.edin2@gmail.com | 12/04 - 01/09 |
| Emmeli Blomkvist | emmeli.blomkvist.2280@student.uu.se | 12/04 - 01/09 |
| Karl Widborg Kortelainen | karl.widborgkortelainen.7261@student.uu.se | 12/04 - 01/09 |
| Mauritz Hamrin Sverredal | mauritz.hamrinsverredal.0549@student.uu.se| 12/04 - 01/09 |
| Thomas Nohrin | thomas.nohrin.8519@student.uu.se | 12/04 - 01/09 |

## 2 Quantification

- Project start date: 2023/12/04
- Project end date: 2024/01/09

Number of sprints, their start and end dates 3 sprints

- Sprint 1: 12/04 - 12/13
- Sprint 2: 12/13 - 12/20
- Sprint 3: 12/20 - 01/09

- Total number of new lines of C code written excluding tests and preexisting code:
Approx 1296 lines (we didn't count the exact number of demo lines added/changed)
- Total number of lines of test code:
554 (not including existing tests for inlupp2)
- Total number of lines of “script code” (e.g., make files, - - - Python scripts for generating test data, etc.): 97 lines (just Makefile)
- Total number of hours worked by the team: 275 hours
- Total number of git commits: 227
- Total number of pull requests: 78
- Total number of GitHub issues: 72

## 3 Process
### 3.1 Inception

For our development process, we chose a mix of scrum/kanban because that was what we had the most knowledge of. We created a kanban board on GitHub projects where we tracked our tasks (we chose this instead of Trello since we had everything else on GitHub).
### 3.2 Implementation

In this section, discuss:

**What you actually implemented of the process**

During this project we used agile working methadology in a kanban system. This means that we were focused on issues and solving a small issue in a bigger area. We did this by using issue tracking in github together with the project board that github also supplies with each repository.

We started each sprint by planning the sprint. This meant mostly creating small enough issues that each group member could claim and therefore complete it. This was followed by a pull request were each issue was linked to. This made it easy to know and perform a code review.

**Strengths and weaknesses with your implementation of your chosen process**

A strength with this working process that the code that is produces is failsafe. This is possible because of thourough planning making it clear what, how and why an issue should be completed. In other words issues were created with other issues in mind so that they would work together in a working program.

A weakness is that this way of working is very administrative heavy. It requires a lot of planning and also a lot of time reviewing pull-requests, viewing issues, moving issues and describing solutions for each issue. While this is a great way of making it possible to work in larger groups it slowens down development speed in cost of good code.

**What you would do differently if you were to start over tomorrow**

We would probably do more thourough testing early on to catch bugs earlier and save us some stress. For example we found out very late that default destructors was not working as we expected AFTER we integrated the program into our demo. This created huge problems on the last days of the project were we were getting bugs that were difficult to solve. By being 100% sure that each function worked as expected this could have been prevented.

**What successes you would attempt to repeat if you were to start over tomorrow**

It's pretty clear that we were not doing the tests properly enough and when we discovered it, it was way to late. The planning of the project have been excellent, just not the execution.

**How plans were made, key plans, and to what extent your plans were followed**

In the beginning of each sprint we had multiple meetings were all group members discussed what the end goal of the sprint should be and how we reach that end goal. We usually had a bigger goal for example: truly understand the problem, creating refmem and integration of warehouse. These huge tasks were divided into smaller issues that made i

**How decision making happened, key decisions, and whether they were followed**

We decided fairly equally in our group meetings and also had multiple stand up meetings each week were we could discuss problems we had with ongoing tickets. Theese meetings were lead by a scrum master, Thomas, that we assigned in the beginning of the project. We made the decision that everyone should take decisions but Thomas was the one leading the meeting.

**How did you attack the Christmas break problem with planning?**

We didn't schedule any meetings between 22/12 - 29/12 but members were free to work on tickets.

## 4 Use of Tools

We used GCC, GDB, Clang, GCC/Clang address and undefined behaviour sanitizers, valgrind, leaks (macOS equivalent of valgrind), CUnit, VSCode, lcov, gcov, Git, GitHub, GitHub Actions, Discord, zoom, terminal commands e.g. rm, find.

**What use you had of those tools**
We used several tools such as GCC, GDB, valgrind etc. to find bugs and memory leaks in our code.
Other tools like git and github were used for centralising our codebase and maintaining integrity in the main code and GitHub was used for pull requests and issue tracking.
We also used tools such as discord and zoom for communication purposes.

**If there were any tools you were lacking**
We think all the tools we used served their purpose at some point.
Some tools were tedious at times but had their uses.

**Tools you would rather not use in the future**
This point varies from member to member depending on how much previous experience we had with the different tools.
For example some of us were more comfortable with using GDB while others found it difficult/inefficient to use.
Same thing goes for Make, Git etc.

## 5 Communication, Cooperation and Coordination
In general the communication within the group went smoothly. With tools such as discord we could quickly get answers to our questions and cooperate to solve the problems that occured. During the earlier stages of the project, much of the work could be done separately but when we got to the integration we had to coooperate and work together which went well.

Github projects was a great help for coordinating technical tasks as we could write tickets for different parts to implement and then assign them to people so that it was easy to see what was being worked on and by who.

Before the break we had a final meeting to make sure everyone was on the same page. That way we could work separately during the break, each in their own pace, and then have a meeting after the break to see where everyone was at.
We learned the hard way that when working in a larger group and with a strict structure (code reviews, branches, pull request etc.) it takes a lot more time than we thought to make progress.

## 6 Work Breakdown Structure
We divided the project into three sprints.
The first sprint was just planning and preparing for the project which we gave slightly more than a week as all of us had some other things to take care of as well.
In the second sprint we wanted a working prototype of the reference counter which meant we should be able to run the example without any crashes or memory leaks.
We figured this prototype should take about a week to implement and that ended up being the case.
The last and biggest (probably a bit too big) sprint consisted of implementing default destructor and cascade limit, integrating our code into a previous lab and writing the reports.
Since this was the last sprint we did not really consider how long it should take as long as it was done before the project was due.
At first this went smoothly but we later realised default destructor was not working as intended which set us back quite a lot.

The tasks were of varying sizes (implementing a function, writing a test, documenting etc.) and we figured that it should not be a problem since the smaller tasks would be faster to complete compared to the larger tasks.
That way, while some of us completed more small tasks quickly, others worked longer on bigger tasks and as long as everyone took a new task when they were done with one it would more or less balance out.
As previously mentioned, at first most of the work was done separately and when we started integrating we started working a lot together to solve the problems that occured.
We found it easy to estimate the required time for some tasks such as documentation while other tasks such as writing functions were very difficult to estimate as problems often occured.

We divided the tasks by using git actions and making tickets where we each could assign ourself a ticket and work on that particular task. We therefore did work a lot alone, however we also worked in pairs when tasks was more difficult.The actual tickets that we made was of varying degrees of difficulty, some was just making a test for a function. Others was writing a whole function, which could take more time depending on which function. We tried to make sure nobody was overwhelmed since if you were stuck on a task or ticket then you could make a pull request and ask for changes or create more tickets for your own ticket you have assigned to divide the workload up. But in certain cases using this ticket system could make it for others more work heavy since some tickets require more time. We have a Pie chart below where we have estimated our time spent based upon task activities. We also have a Burndown chart below:

[Burndown chart](https://docs.google.com/spreadsheets/d/1CDri7Tw2aqk7cFixoFhbtQ6hchg2L-9VS7vOs4Leoy8/edit?usp=sharing)

[Pie chart](https://docs.google.com/document/d/1lU-7vLMKHXGUwjhA6sCcH_reFywJtCFXtVb_h29WA6E/edit)

## 7 Reflection

In this section, discuss briefly:

**How confident you are that have actually implemented the right thing (why, how, etc.)**

- Refmem: 6/7 Since we have good test coverage and during our implementation of the demo refmem.c seemed to be working properly.
- Demo: 3/7 (It's almost working but not quite)

**On a scale 1–7 (7 is best), rate your satisfaction with your process and provide justification for that number**

5: it was quite tedious to create tickets for different tasks but it was a good way of keeping track of different tasks.

**On a scale 1–7 (7 is best), rate your satisfaction with your delivered product and provide justification for that number**

We would say it lands on a 3/7. This is mainly because of two things. First of all we did not manage to completely integrate our reference counter into another project but were very close.
Second is that because we got stuck on integration, we had no time to optimize our code meaning it is most certainly not as effective as it could be.

**On a scale 1–7 (7 is best), rate your satisfaction with your quality assurance and provide justification for that number**

We would say our quality assurance is a 6/7. We have done some testing and also reviewed all the code.
These two factors and seeing part of the integration work (linked list and hash table succesfully integrated our system) leads us to being quite confident in our program actually working (although not very efficiently).


**What does the team consider its biggest win?**

Most of learned a lot working in an agile way. While it might be a little bit to administrative heavy It does ensure great code. It might be the best way to work together in a larger group since it ensures that everyone works on their own issue and that everyone is working to achieve a common goal.

**What does the team consider its biggest fail?**

Not having a working demo.

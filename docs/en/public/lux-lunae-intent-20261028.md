<a id="왜-럭스루나에인가"></a>

# Why Lux Lunae?

<!-- language-nav:start -->
[한국어](../../public/lux-lunae-intent-20261028.md) | [English](lux-lunae-intent-20261028.md)
<!-- language-nav:end -->

<a id="달빛과-두-게임"></a>

## Moonlight and two games

After leaving my last company in 2025, I rode a motorcycle around the country. I traveled unfamiliar regional roads and empty paths through fields. In darkness so deep I could not see my own hands, judging the road by moonlight and my headlight, I felt as though only the bike and I existed. After that journey, “moonlight” became an unusual symbol in my life.

Before I began restoring HeavyMach, when Ace Combat 8 was taking preorders about a month before release, my mental state was at its worst. Hearing about the release, I thought, “Let's die a little later,” and “At least stay alive until that comes out.”

My journey under moonlight and Namco naming its boss squadron Lux Lunae were independent coincidences. **Lux Lunae**... means “moonlight” in Latin.

Playing Ace Combat 8, I found the overall quality I had waited for. Lux Lunae combined well-designed appearances, ferocious skill to match and actors' dialogue that was enjoyable to hear. The whole game was well made, and Lux Lunae, one of its symbols, contributed to that.

In the developer's statement, I wrote that HeavyMach gave me another nine years and three months of life. I think Ace Combat 8 kept me alive at least until after its release. Lux Lunae was a symbol I could bring from that game, particularly one of its most representative villains.

<a id="게임을-하고-달라진-일"></a>

## What changed after playing a game

A Korean community post is titled “Our marriage, saved by Death Stranding.” Its author wrote that they lost their two-month-old child and that his wife could not leave home for over a year afterward. Two days after they finished the game together, she took a step outside the front door. Later she reached the elevator, then the ground floor. He ended the post by saying he had sent Hideo Kojima a long email. [Post and attached original](https://mlbpark.donga.com/mp/b.php?b=bullpen&id=202606240116557565)

What I thought about in that account was the change in everyday life after experiencing a game. HeavyMach influenced my choice to become a developer. I changed careers in July 2017. Ace Combat 8 became something whose release I wanted to wait for and experience myself. The two games affected different periods of my life.

Restoring HeavyMach and putting Lux Lunae into it brought the roles both games played in my life into one playable game. My memory of Lux Lunae includes not just black aircraft and gold decoration, but the threat and tension of fighting them. So I considered both their appearance and the combat they would create.

<a id="검은색과-금색"></a>

## Black and gold

I wanted Lux Lunae colors on the F-22, Su-57 and MiG-31. Accurately depicting black airframes, gold stripes, gold cockpits and tail emblems on a small mobile game screen was difficult. Still, those elements had to remain for the visual homage I intended to come through. Simply drawing black fighters was not enough.

<a id="보스전의-난이도와-긴장감"></a>

## Boss-fight difficulty and tension

The F-22, Su-57 and MiG-31 felt the most villainous among Ace Combat 8's Lux Lunae aircraft. I do regret that their designs are broadly similar. Perhaps one could have had a different shape, like an F-16 or Typhoon, for more variety.

I wanted to recreate the threat and appeal I felt facing them. I think twisted, powerful but compelling villains stay in public memory. I felt that appeal in Josh Brolin's character in Sicario and the F-22 in Transformers.

There was also something I wanted to change as game design. Before my first job at the magazine, I passed the application-screening stage for a game-design position at Netmarble. The original boss fights combined a fairly difficult boss with ordinary enemies seen in other stages. I found that arrangement somewhat plain.

I wanted minimal additions using the original assets and unit-behavior code while preserving the existing play experience. There were two goals: increase boss-fight difficulty, and create tension by introducing hard-to-kill enemies before and during boss fights. They would share the existing enemies' design language while feeling different in appearance and performance.

Beyond making players pay more attention to movement and evasion, I wanted the tension of thinking, “I need a special attack for this, or a deep breath and some concentration.” To me, the original bosses could be beaten with control, standard weapons and enough time. After adding the new enemies, even I—who rarely used special attacks except when I could not be bothered—started using them actively to clear missions.

I am considering irregular behavior such as leaving the map and reappearing.

<a id="텍스트-지시로-어디까지-만들-수-있는가"></a>

## How much can text instructions produce?

Technically, I wanted to see how accurately Codex could implement a design from text instructions alone. I also wanted to see whether it could reuse existing source to build working features and existing assets to make working visual effects. From design to unit behavior, I wanted to observe what proportion of the output ignored my instructions.

Unlike my other projects, this had no separately configured harness or agents. Another technical question was how much the model's performance and existing source alone could produce.

<a id="움직임과-무기-빠른-진입"></a>

## Movement, weapons and fast entry

I checked the results of code reuse through play. In terms of preserving the original feel with minimal changes or tuning to its source, I think Lux Lunae's movement and weapon procedures stay within the original game's behavior.

At first, I disliked aircraft suddenly appearing in midair and asked for a fix. They then spawned off-screen and flew rapidly inward. I had not directly requested that fast entry. But it somewhat recalled Lux Lunae's high-speed hit-and-run attacks and vicious turning fights in Ace Combat 8, so I kept it. I think the squadron flying in bunched together needs correcting.

<a id="그림-작업에서-본-한계"></a>

## Limits I saw in the artwork

In aesthetics, visual elements and tone, I found Codex overly dependent on the OpenAI image model's style. That happened even with working source, assets and visual references such as sprites.

Lux Lunae was a relatively lucky case. For the Yak, Harrier and F35, I assumed a person would understand the original design's tone and depict them similarly, since the references were already sufficient. Instead, Codex brought science-fiction designs and drafts that ignored my instructions. I had to wrestle with it again to get them corrected.

<a id="지금도-고치고-있는-것"></a>

## What I am still changing

Even while working on something personally meaningful, I kept checking how accurately Codex followed my instructions. I judged the results myself: whether it had read and reused existing code and artwork, and whether it had implemented the appearance and behavior I wanted. Both the drafts I made it correct and the fast entry I chose to keep are part of how this game reached its current form.

I still think the bunched-up squadron needs fixing. I am comparing the new ground units' textures with other units and considering attacks that leave the screen and return. My experiences with the two games are part of why I restored this one. How to turn those experiences into play remains something I judge as I build and revise it.

<a id="승인한-도안-3종"></a>

## Three approved designs

These are the final designs with gold cockpits. All three images became aircraft assets in the game.

### F-22

![Final F-22 design: black body, gold stripes, gold cockpit and tail emblem](../../public/media/lux-lunae/F22-gold.png)

### Su-57

![Final Su-57 design: long body, gold cockpit and tail emblem](../../public/media/lux-lunae/Su57-gold.png)

### MiG-31

![Final MiG-31 design: two-seat gold cockpit and tail emblem](../../public/media/lux-lunae/MiG31-gold.png)

<a id="실제-인게임-모습"></a>

## In-game appearance

<a id="직접-플레이한-영상"></a>

### My play recording

![The tank fighting added aircraft in mission 3-4](../../public/media/gameplay-20261027/07-fighters.png)

Mission 3-4. Added fighters gather on the right while the tank's beam reaches toward them.

This is the 1:30–1:43 section of the recording supplied on October 27. [Video file](../../public/media/gameplay-20261027/07-fighters.mp4).

<a id="ipad-보스전"></a>

### iPad boss fight

![The boss and new fighters attacking together on iPad](../../public/media/lux-lunae/ipad-combat.png)

Combat test with the boss and eight reinforcement fighters.

<a id="iphone-보스전"></a>

### iPhone boss fight

![The boss and new fighters attacking together on iPhone](../../public/media/lux-lunae/iphone-combat.png)

<a id="기존-헬기와-크기-비교"></a>

### Size comparison with existing helicopters

![In-game sizes of two original helicopters, F-22, Su-57 and MiG-31](../../public/media/lux-lunae/ipad-size.png)

From top: Heli001, Heli006, F-22, Su-57 and MiG-31. Level and pitched attitudes are shown side by side.

<a id="게임에-적용한-조건"></a>

## Conditions implemented in the game

| Item | Implementation |
|---|---|
| Durability | 765 for all three; 1.7 times the existing Heli maximum of 450 |
| F-22 and Su-57 | Three-shot bursts using the existing single missile |
| MiG-31 | Three pairs of existing missiles, six total |
| Missile speed | 25% faster than the original weapon |
| Firing rate | Initially 25% faster, reduced to 10% after a follow-up instruction |
| Boss fights | 2, 4, 6, then 8 aircraft as missions progress; capped at 8 afterward |
| Before boss fights | Four aircraft in the final world section |
| Total placements | 44 in boss fights and 28 in preceding sections, 72 additions |
| Existing enemies | Original 1,182 entries and 113 VTOL entries preserved |
| Entry | Spawn the entire aircraft off-screen, then fly in; no firing during entry |

Codex's initial implementation made aircraft suddenly appear inside the screen. After I reported it while playing the regular app, entry changed to start off-screen. Missile speed and firing rate were adjusted separately.

[Weapons, missions and test records](../../public/lux-lunae-design-20261027.md) · [Final design manifest](../../public/media/lux-lunae/gold-manifest.json) · [Recording sources and timestamps](../../public/media/gameplay-20261027/media-scenes.json)

<a id="대화-원문"></a>

## Original conversation

[Fourteen original statements on intent and later evaluation](../../references/developer-background/user-lux-lunae-intent-20261028.md)
